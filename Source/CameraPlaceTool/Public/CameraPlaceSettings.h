#pragma once
#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "CameraPlaceSettings.generated.h"

/**
 * Project Settings → Plugins → CameraPlace
 */
UCLASS(Config=EditorPerProjectUserSettings, DefaultConfig, meta=(DisplayName="Camera Place Tool"))
class CAMERAPLACETOOL_API UCameraPlaceSettings : public UDeveloperSettings
{
    GENERATED_BODY()
public:
    // Project Settings 좌측 트리 경로
    virtual FName GetCategoryName() const override { return TEXT("Plugins"); }
    virtual FName GetSectionName()  const override { return TEXT("CameraPlace"); }

    /* ---------- Viewport (앞 고정 배치) ---------- */

    /** 뷰포트 카메라 앞 고정 배치 사용 */
    UPROPERTY(EditAnywhere, Config, Category="Viewport")
    bool bPlaceDirectlyInFrontOfViewport = true;

    /** 카메라 앞 거리(cm) */
    UPROPERTY(EditAnywhere, Config, Category="Viewport", meta=(ClampMin="0.0"))
    float ViewportPlaceDistance = 120.0f;

    /** 위로 살짝 띄우기(cm) */
    UPROPERTY(EditAnywhere, Config, Category="Viewport")
    float ViewportUpOffset = 0.0f;

    /** 앞 고정 위치에서 아래로 스냅(지면 붙이기) */
    UPROPERTY(EditAnywhere, Config, Category="Viewport")
    bool bSnapDownToSurface = true;

    /* ---------- Ray Trace (전방 레이) ---------- */

    /** 전방 레이 길이(cm) */
    UPROPERTY(EditAnywhere, Config, Category="Trace/Raycast", meta=(ClampMin="0.0"))
    float TraceDistance = 100000.0f;

    /** 레이 미히트시 앞 고정 대체 거리(cm) */
    UPROPERTY(EditAnywhere, Config, Category="Trace/Raycast", meta=(ClampMin="0.0"))
    float FallbackDistance = 2000.0f;

    /* ---------- 배치 공통 ---------- */

    /** 표면에서 살짝 띄우기(cm) */
    UPROPERTY(EditAnywhere, Config, Category="Placement", meta=(ClampMin="0.0"))
    float SurfaceOffset = 0.5f;

    /** 표면 노멀에 회전 정렬(경사면에 기울이기) */
    UPROPERTY(EditAnywhere, Config, Category="Placement")
    bool bAlignToSurface = true;

    /** 항상 월드 업(Z+) 유지(= 롤 0, 수직 세우기). 켜져있으면 bAlignToSurface 무시 */
    UPROPERTY(EditAnywhere, Config, Category="Placement")
    bool bKeepWorldUp = false;
};