#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CameraPlaceSettings.generated.h"

UCLASS(Config=EditorPerProjectUserSettings)
class UCameraPlaceSettings : public UObject
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, Config, Category="CameraPlace") float TraceDistance = 100.f; // 1km
    UPROPERTY(EditAnywhere, Config, Category="CameraPlace") float FallbackDistance = 2000.f; // 미히트 시
    UPROPERTY(EditAnywhere, Config, Category="CameraPlace") float SurfaceOffset = 0.5f; // cm
    UPROPERTY(EditAnywhere, Config, Category="CameraPlace") bool bAlignToSurface = true; // 표면 노멀에 업 정렬
    UPROPERTY(EditAnywhere, Config, Category="CameraPlace") bool bKeepWorldUp = false; // 월드 업 유지(체크 시 bAlignToSurface 무시)
};