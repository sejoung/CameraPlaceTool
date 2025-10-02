#include "UI_CameraPlaceMenu.h"
#include "CameraPlaceSubsystem.h"
#include "ToolMenus.h"
#include "LevelEditor.h"
#include "Framework/Commands/InputChord.h"
#include "Framework/Commands/UICommandList.h"
#include "Framework/Commands/Commands.h"
#include "Styling/AppStyle.h"
#include "Editor.h"

#define LOCTEXT_NAMESPACE "CameraPlaceTool"

// ✅ 이 오너 이름으로 등록한 메뉴 항목을 전부 한 번에 해제할 수 있습니다.
static FToolMenuOwner GCameraPlaceOwner("CameraPlaceTool");

// 커맨드 정의
class FCameraPlaceCommands : public TCommands<FCameraPlaceCommands>
{
public:
    FCameraPlaceCommands()
    : TCommands<FCameraPlaceCommands>(
        TEXT("CameraPlaceTool"),
        LOCTEXT("CameraPlace", "CameraPlaceTool"),
        NAME_None,
        FAppStyle::GetAppStyleSetName())
    {}

    virtual void RegisterCommands() override
    {
        UI_COMMAND(PlaceFromCamera, "Place From Camera",
            "Place the selected asset from camera forward",
            EUserInterfaceActionType::Button,
            FInputChord(EModifierKey::Control | EModifierKey::Alt, EKeys::P));
    }

    TSharedPtr<FUICommandInfo> PlaceFromCamera;
};

static TSharedPtr<FUICommandList> GCmdList;
static FDelegateHandle GStartupHandle;

static void ExecutePlaceFromCamera()
{
    if (GEditor)
    {
        if (auto* Sys = GEditor->GetEditorSubsystem<UCameraPlaceSubsystem>())
        {
            Sys->PlaceSelectedFromCamera();
        }
    }
}

void RegisterCameraPlaceMenus()
{
    // 커맨드 등록 + 액션 맵
    FCameraPlaceCommands::Register();

    GCmdList = MakeShared<FUICommandList>();
    GCmdList->MapAction(
        FCameraPlaceCommands::Get().PlaceFromCamera,
        FExecuteAction::CreateStatic(&ExecutePlaceFromCamera));

    // 툴바/컨텍스트 메뉴 확장 (엔진 초기화 후 실행 보장)
    GStartupHandle = UToolMenus::RegisterStartupCallback(
        FSimpleMulticastDelegate::FDelegate::CreateLambda([]()
    {
        UToolMenus* Menus = UToolMenus::Get();

        // 레벨 에디터 툴바
        if (UToolMenu* Toolbar = Menus->ExtendMenu("LevelEditor.LevelEditorToolBar"))
        {
            FToolMenuSection& Section =
                Toolbar->AddSection("CameraPlace", LOCTEXT("Section","CameraPlace"));
            Section.AddEntry(
                FToolMenuEntry::InitToolBarButton(
                    GCameraPlaceOwner,                                   // ✅ Owner 지정
                    FCameraPlaceCommands::Get().PlaceFromCamera));
        }

        // 콘텐츠 브라우저 우클릭
        if (UToolMenu* CB = Menus->ExtendMenu("ContentBrowser.AssetContextMenu"))
        {
            FToolMenuSection& Section =
                CB->AddSection("CameraPlaceCtx", LOCTEXT("Ctx","CameraPlaceTool"));
            Section.AddMenuEntry(GCameraPlaceOwner, FCameraPlaceCommands::Get().PlaceFromCamera);
        }
    }));
}

void UnregisterCameraPlaceMenus()
{
    // 스타트업 콜백 해제
    if (GStartupHandle.IsValid())
    {
        UToolMenus::UnRegisterStartupCallback(GStartupHandle);
        GStartupHandle.Reset();
    }

    // 이 Owner로 등록된 모든 메뉴 항목 제거
    UToolMenus::UnregisterOwner(GCameraPlaceOwner);   // ✅ nullptr 금지!

    GCmdList.Reset();
    FCameraPlaceCommands::Unregister();
}

#undef LOCTEXT_NAMESPACE