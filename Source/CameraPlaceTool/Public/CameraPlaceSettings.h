#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CameraPlaceSettings.generated.h"

UCLASS(Config=EditorPerProjectUserSettings)
class UCameraPlaceSettings : public UObject
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, Config, Category="CameraPlace") float TraceDistance = 100.f; 
    UPROPERTY(EditAnywhere, Config, Category="CameraPlace") float FallbackDistance = 200.f; // 미히트 시
    UPROPERTY(EditAnywhere, Config, Category="CameraPlace") float SurfaceOffset = 0f; // cm
    UPROPERTY(EditAnywhere, Config, Category="CameraPlace") bool bAlignToSurface = true; // 표면 노멀에 업 정렬
    UPROPERTY(EditAnywhere, Config, Category="CameraPlace") bool bKeepWorldUp = false; // 월드 업 유지(체크 시 bAlignToSurface 무시)
	UPROPERTY(EditAnywhere, Config, Category="CameraPlace|Viewport") bool bPlaceDirectlyInFrontOfViewport = true; // 뷰포트 앞 고정 배치 사용
	UPROPERTY(EditAnywhere, Config, Category="CameraPlace|Viewport", meta=(ClampMin="0.0")) float ViewportPlaceDistance = 120.f;         // 카메라 앞 거리(cm)
	UPROPERTY(EditAnywhere, Config, Category="CameraPlace|Viewport") float ViewportUpOffset = 0.f;                // 필요시 위로 살짝 띄우기
};