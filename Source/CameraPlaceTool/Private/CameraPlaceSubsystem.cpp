#include "CameraPlaceSubsystem.h"
#include "CameraPlaceSettings.h"

#include "Editor.h"
#include "EditorViewportClient.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraActor.h"
#include "Engine/StaticMeshActor.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"

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

    // 트레이스
    const UCameraPlaceSettings* S = GetDefault<UCameraPlaceSettings>();
    const FVector Start = Cam.GetLocation();
    const FVector End = Start + Cam.GetRotation().GetForwardVector() * S->TraceDistance;

    FHitResult Hit; FCollisionQueryParams Params(SCENE_QUERY_STAT(CameraPlaceTrace), true);
    World->LineTraceSingleByChannel(Hit, Start, End, ECC_WorldStatic, Params);

    // 배치할 자산
    UClass* SpawnClass = nullptr; UStaticMesh* Mesh = nullptr;
    UObject* Asset = GetSelectedPlacableAsset(SpawnClass, Mesh);
    if (!Asset) { UE_LOG(LogTemp, Warning, TEXT("No placable asset selected")); return; }

    const FTransform T = MakePlacementTransform(Cam, Hit.bBlockingHit ? &Hit : nullptr);

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
}