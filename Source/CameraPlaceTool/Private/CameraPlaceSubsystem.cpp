#include "CameraPlaceSubsystem.h"
#include "CameraPlaceSettings.h"
#if WITH_EDITOR
#include "Selection.h"                 // USelection
#include "LevelEditorViewport.h"       // FLevelEditorViewportClient
#include "UnrealClient.h"              // FViewportCameraTransform
#include "EditorViewportClient.h"      // (이미 있었으면 유지)
#include "Editor.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraActor.h"
#include "Engine/StaticMeshActor.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#endif

static bool SnapDownToSurface(UWorld* World, const FVector& FromLocation, float DownDistance, float UpStartOffset,
                              FVector& OutSnappedLocation, FVector& OutSurfaceNormal)
{
    const FVector Up = FVector::UpVector;
    const FVector Start = FromLocation + Up * UpStartOffset;        // 살짝 위에서 시작
    const FVector End   = Start - Up * DownDistance;                // 아래로 길게

    FHitResult Hit;
    FCollisionQueryParams Q(SCENE_QUERY_STAT(CameraPlace_SnapDown), false);
    if (World->LineTraceSingleByChannel(Hit, Start, End, ECC_WorldStatic, Q) && Hit.bBlockingHit)
    {
        OutSnappedLocation = Hit.ImpactPoint;
        OutSurfaceNormal   = Hit.ImpactNormal;
        return true;
    }
    return false;
}

static UWorld* EditorWorld()
{
    return GEditor ? GEditor->GetEditorWorldContext().World() : nullptr;
}

void UCameraPlaceSubsystem::Initialize(FSubsystemCollectionBase&)
{
}

bool UCameraPlaceSubsystem::GetCameraTransform(FTransform& OutCam) const
{
    // 1) 선택된 CameraActor 우선
    if (GEditor)
    {
        TArray<AActor*> SelectedActors; GEditor->GetSelectedActors()->GetSelectedObjects<AActor>(SelectedActors);
        for (AActor* A : SelectedActors)
        {
            if (Cast<ACameraActor>(A)) { OutCam = A->GetActorTransform(); return true; }
        }
    }

    // 2) 현재 레벨 에디터 뷰포트 카메라
    if (GCurrentLevelEditingViewportClient)
    {
        const FViewportCameraTransform& V = GCurrentLevelEditingViewportClient->GetViewTransform();
        OutCam = FTransform(V.GetRotation(), V.GetLocation());
        return true;
    }
    return false;
}

UObject* UCameraPlaceSubsystem::GetSelectedPlacableAsset(UClass*& OutSpawnClass, UStaticMesh*& OutMesh) const
{
    OutSpawnClass = nullptr; OutMesh = nullptr;

    FContentBrowserModule& CB = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
    TArray<FAssetData> Selected; CB.Get().GetSelectedAssets(Selected);
    if (Selected.Num() == 0) return nullptr;

    const FAssetData& AD = Selected[0];
    UObject* Asset = AD.GetAsset();

    // 1) 블루프린트 클래스
    if (UBlueprint* BP = Cast<UBlueprint>(Asset))
    {
        if (BP->GeneratedClass && BP->GeneratedClass->IsChildOf(AActor::StaticClass()))
        {
            OutSpawnClass = BP->GeneratedClass;
            return Asset;
        }
    }
    // 2) 직접 UClass 선택
    if (UClass* C = Cast<UClass>(Asset))
    {
        if (C->IsChildOf(AActor::StaticClass()))
        {
            OutSpawnClass = C; return Asset;
        }
    }
    // 3) StaticMesh → AStaticMeshActor 스폰
    if (UStaticMesh* SM = Cast<UStaticMesh>(Asset))
    {
        OutMesh = SM; return Asset;
    }
    return nullptr;
}

FTransform UCameraPlaceSubsystem::MakePlacementTransform(const FTransform& Cam, const FHitResult* Hit) const
{
    const UCameraPlaceSettings* S = GetDefault<UCameraPlaceSettings>();

    FVector Location; FRotator Rotation = Cam.Rotator();

    if (Hit && Hit->bBlockingHit)
    {
        Location = Hit->ImpactPoint + Hit->ImpactNormal * S->SurfaceOffset;
        if (S->bKeepWorldUp)
        {
            // 월드 업을 유지하고 카메라의 yaw/pitch만 사용
            Rotation = FRotator(Rotation.Pitch, Rotation.Yaw, 0.f);
        }
        else if (S->bAlignToSurface)
        {
            // 표면 노멀을 업으로, 카메라 전방을 표면 평면에 투영하여 회전 구성
            const FVector Up = Hit->ImpactNormal.GetSafeNormal();
            FVector Fwd = Cam.GetRotation().GetForwardVector();
            // 표면 평면으로 투영하여 전방이 표면을 따라가게
            Fwd = (Fwd - FVector::DotProduct(Fwd, Up)*Up).GetSafeNormal();
            const FVector Right = FVector::CrossProduct(Up, Fwd).GetSafeNormal();
            const FMatrix M(Fwd, Right, Up, FVector::ZeroVector);
            Rotation = M.Rotator();
        }
    }
    else
    {
        Location = Cam.GetLocation() + Cam.GetRotation().GetForwardVector() * S->FallbackDistance;
        Rotation = FRotator(Rotation.Pitch, Rotation.Yaw, 0.f);
    }

    return FTransform(Rotation, Location);
}

void UCameraPlaceSubsystem::PlaceSelectedFromCamera()
{
    UWorld* World = EditorWorld(); if (!World) return;

    FTransform Cam; if (!GetCameraTransform(Cam)) return;

    const UCameraPlaceSettings* S = GetDefault<UCameraPlaceSettings>();

    // ✅ 뷰포트 카메라 앞 고정 배치 (레이캐스트 생략)
    if (S->bPlaceDirectlyInFrontOfViewport)
    {
        // 배치할 자산 가져오기
        UClass* SpawnClass = nullptr; UStaticMesh* Mesh = nullptr;
        UObject* Asset = GetSelectedPlacableAsset(SpawnClass, Mesh);
        if (!Asset) { UE_LOG(LogTemp, Warning, TEXT("No placable asset selected")); return; }

        const FVector Fwd = Cam.GetRotation().GetForwardVector();
        FVector Location  = Cam.GetLocation() + Fwd * S->ViewportPlaceDistance
                                          + FVector::UpVector * S->ViewportUpOffset;

        // 카메라가 보는 방향(Yaw/Pitch만, Roll 0)
        const FRotator Rotation(Cam.Rotator().Pitch, Cam.Rotator().Yaw, 0.f);
        const FTransform T(Rotation, Location);
		if (S->bSnapDownToSurface)
		{
    		FVector Snapped, SurfaceN;
    		if (SnapDownToSurface(World, Location, /*DownDistance*/100000.0f, /*UpStartOffset*/50.0f, Snapped, SurfaceN))
    		{
        		// 표면에 살짝 띄우기
        		Location = Snapped + SurfaceN * S->SurfaceOffset;

        		// 회전: 표면에 정렬할지, 월드 업 유지할지 선택
        		if (S->bAlignToSurface && !S->bKeepWorldUp)
        		{
            		// 카메라 전방을 표면 평면에 투영 → 회전 생성
            		const FVector Up = SurfaceN.GetSafeNormal();
                    FVector FwdOnPlane = (Fwd - FVector::DotProduct(Fwd, Up)*Up).GetSafeNormal();
                    const FVector Right = FVector::CrossProduct(Up, FwdOnPlane).GetSafeNormal();
                    const FMatrix M(FwdOnPlane, Right, Up, FVector::ZeroVector);
                    const FRotator Rotation = M.Rotator();

                }
                else
                {
                    // 표면은 쓰되 롤은 0(월드 업 유지)
                    const FRotator Rotation(Cam.Rotator().Pitch, Cam.Rotator().Yaw, 0.f);
                    // Spawn 시: const FTransform T(Rotation, Location);
                }
            }
        }

        // 최종 스폰
        FRotator FinalRot = (S->bAlignToSurface && !S->bKeepWorldUp)
            ? /* 위 정렬 로직에서 만든 Rotation */ FRotator(Cam.Rotator().Pitch, Cam.Rotator().Yaw, 0.f)
            : FRotator(Cam.Rotator().Pitch, Cam.Rotator().Yaw, 0.f);

        const FTransform T(FinalRot, Location);


        AActor* NewActor = nullptr;
        if (SpawnClass)
        {
            NewActor = World->SpawnActor<AActor>(SpawnClass, T);
        }
        else if (Mesh)
        {
            AStaticMeshActor* SMA = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), T);
            if (SMA)
            {
                SMA->GetStaticMeshComponent()->SetStaticMesh(Mesh);
                SMA->SetMobility(EComponentMobility::Movable);
                NewActor = SMA;
            }
        }

        if (NewActor)
        {
            GEditor->SelectNone(false, true, false);
            GEditor->SelectActor(NewActor, true, true, true);
            GEditor->NoteSelectionChange();
        }
        return; // ✅ 여기서 끝 — 아래 레이캐스트 로직은 타지 않음
    }

    // --- 기존 레이캐스트 모드 유지 ---
    const FVector Start = Cam.GetLocation();
    const FVector End   = Start + Cam.GetRotation().GetForwardVector() * S->TraceDistance;
    FHitResult Hit; FCollisionQueryParams Params(SCENE_QUERY_STAT(CameraPlaceTrace), true);
    World->LineTraceSingleByChannel(Hit, Start, End, ECC_WorldStatic, Params);
}