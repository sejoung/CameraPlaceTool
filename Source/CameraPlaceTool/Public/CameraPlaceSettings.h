#pragma once
#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "CameraPlaceSettings.generated.h"

UCLASS(Config=EditorPerProjectUserSettings, DefaultConfig, meta=(DisplayName="Camera Place Tool"))
class CAMERAPLACETOOL_API UCameraPlaceSettings : public UDeveloperSettings
{
    GENERATED_BODY()
public:
    // 카테고리 위치 (Project Settings 창의 좌측 트리 경로)
    virtual FName GetCategoryName() const override { return TEXT("Plugins"); }
    virtual FName GetSectionName() const override { return TEXT("CameraPlace"); }

    /** 뷰포트 앞 고정 배치 사용 */
    UPROPERTY(EditAnywhere, Config, Category="Viewport")
    bool bPlaceDirectlyInFrontOfViewport = true;

    /** 배치 거리(cm) */
    UPROPERTY(EditAnywhere, Config, Category="Viewport", meta=(ClampMin="0.0"))
    float ViewportPlaceDistance = 120.0f;

    /** 바닥에 스냅 */
    UPROPERTY(EditAnywhere, Config, Category="Viewport")
    bool bSnapDownToSurface = true;

    /** 표면과 살짝 띄우기(cm) */
    UPROPERTY(EditAnywhere, Config, Category="Placement")
    float SurfaceOffset = 0.5f;

    /** 표면 노멀에 정렬 */
    UPROPERTY(EditAnywhere, Config, Category="Placement")
    bool bAlignToSurface = true;

    /** 항상 월드업 유지 */
    UPROPERTY(EditAnywhere, Config, Category="Placement")
    bool bKeepWorldUp = false;
};