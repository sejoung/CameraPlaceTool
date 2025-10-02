# CameraPlaceTool

에디터 카메라의 전방을 기준으로 선택한 어셋을 배치하고 방향을 맞추는 UE5 에디터 플러그인.

## 설치
- 프로젝트의 `Plugins/CameraPlaceTool/`에 본 폴더를 복사.
- 플러그인 활성화 후 에디터 재시작.

## 사용법
1. Content Browser에서 배치할 **블루프린트(Actor)** 또는 **Static Mesh**를 선택.
2. 씬에서 CameraActor를 선택하거나 (없으면 현재 뷰포트 카메라 사용)
3. 툴바 버튼 **Place From Camera** 클릭(단축키 `Ctrl+Alt+P`).

## 옵션
`Editor Preferences → Project → CameraPlace`
- TraceDistance: 레이 길이(기본 1000m)
- FallbackDistance: 히트 실패 시 전방 배치 거리
- SurfaceOffset: 표면에서 띄우는 정도(cm)
- AlignToSurface: 표면 노멀에 맞춰 기울이기
- KeepWorldUp: 월드 업(Z+) 유지(AlignToSurface보다 우선)

## 메모
- 더 정교한 접지(바닥) 배치는 챔퍼/피벗 오프셋을 고려해 Z 오프셋을 추가로 적용하세요.
- 스켈레탈 메시/다른 타입 지원은 스폰 로직을 확장하세요.

