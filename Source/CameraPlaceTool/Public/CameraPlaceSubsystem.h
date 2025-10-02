#pragma once
#include "CoreMinimal.h"

#include "CameraPlaceSubsystem.generated.h"
#if WITH_EDITOR
UCLASS()
class UCameraPlaceSubsystem : public UEditorSubsystem
{
    GENERATED_BODY()
public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    // 메인 엔트리: 선택 자산을 카메라 전방에 배치
    UFUNCTION(CallInEditor) void PlaceSelectedFromCamera();

private:
    bool GetCameraTransform(FTransform& OutCamTransform) const; // 선택 카메라 또는 뷰포트
    UObject* GetSelectedPlacableAsset(UClass*& OutSpawnClass, UStaticMesh*& OutStaticMesh) const;
    FTransform MakePlacementTransform(const FTransform& Cam, const FHitResult* Hit) const;
};
#endif