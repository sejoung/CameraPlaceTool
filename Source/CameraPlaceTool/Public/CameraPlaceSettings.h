#pragma once
#include "CoreMinimal.h"
#include "CameraPlaceSettings.generated.h"

UCLASS(Config=EditorPerProjectUserSettings)
class UCameraPlaceSettings : public UObject
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, Config, Category="CameraPlace|Viewport", meta=(ClampMin="0.0"))
    float ViewportPlaceDistance = 120.0f;

    UPROPERTY(EditAnywhere, Config, Category="CameraPlace|Viewport")
    float ViewportUpOffset = 0.0f;

    UPROPERTY(EditAnywhere, Config, Category="CameraPlace|Viewport")
    bool bPlaceDirectlyInFrontOfViewport = true;

    // 이미 있던 값들도 동일 규칙으로(예: 100000.0f, 2000.0f, 0.5f 등) ASCII로 다시 타이핑
    UPROPERTY(EditAnywhere, Config, Category="CameraPlace")
    float TraceDistance = 100000.0f;

    UPROPERTY(EditAnywhere, Config, Category="CameraPlace")
    float FallbackDistance = 2000.0f;

    UPROPERTY(EditAnywhere, Config, Category="CameraPlace")
    float SurfaceOffset = 0.5f;

    UPROPERTY(EditAnywhere, Config, Category="CameraPlace")
    bool bAlignToSurface = true;

    UPROPERTY(EditAnywhere, Config, Category="CameraPlace")
    bool bKeepWorldUp = false;
};