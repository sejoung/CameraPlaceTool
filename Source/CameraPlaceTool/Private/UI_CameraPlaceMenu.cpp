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

static FToolMenuOwner GCameraPlaceOwner("CameraPlaceTool"); // ✅ Owner 객체
static TSharedPtr<FUICommandList> GCmdList;
static FDelegateHandle GStartupHandle;

// 커맨드 정의
class FCameraPlaceCommands : public TCommands<FCameraPlaceCommands>
{
public:
    FCameraPlaceCommands()
    : TCommands<FCameraPlaceCommands>(
        TEXT("CameraPlaceTool"),
        LOCTEXT("CameraPlace","CameraPlaceTool"),
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

static void ExecutePlaceFromCamera()
{
    if (GEditor)
    {
        if (auto* Sys = GEditor->GetEditorSubsystem<UCameraPlaceSubsystem>())
            Sys->PlaceSelectedFromCamera();
    }
}

void RegisterCameraPlaceMenus()
{
    FCameraPlaceCommands::Register();

    GCmdList = MakeShared<FUICommandList>();
    GCmdList->MapAction(
        FCameraPlaceCommands::Get().PlaceFromCamera,
        FExecuteAction::CreateStatic(&ExecutePlaceFromCamera));

    // 엔진 메뉴 초기화 이후에 메뉴 확장
    GStartupHandle = UToolMenus::RegisterStartupCallback(
        FSimpleMulticastDelegate::FDelegate::CreateLambda([]()
    {
        UToolMenus* Menus = UToolMenus::Get();

        // 레벨 에디터 툴바
        if (UToolMenu* Toolbar = Menus->ExtendMenu("LevelEditor.LevelEditorToolBar"))
        {
            FToolMenuSection& Section =
                Toolbar->AddSection("CameraPlace", LOCTEXT("Section","CameraPlace"));

            // ✅ 올바른 시그니처: CommandInfo만 넘김
            FToolMenuEntry Entry = FToolMenuEntry::InitToolBarButton(
                FCameraPlaceCommands::Get().PlaceFromCamera);

            // 필요하면 Owner 지정
            Entry.Owner = GCameraPlaceOwner;
            Entry.SetCommandList(GCmdList);    
            Section.AddEntry(Entry);
        }

        // StaticMesh 전용 컨텍스트 메뉴에도 추가 (로그 경로)
        if (UToolMenu* CBMesh = Menus->ExtendMenu("ContentBrowser.AssetContextMenu.StaticMesh"))
        {
            FToolMenuSection& Section = CBMesh->AddSection("CameraPlaceCtx", LOCTEXT("CtxSM","CameraPlaceTool(StaticMesh)"));
            FToolMenuEntry CtxEntry = FToolMenuEntry::InitMenuEntry(FCameraPlaceCommands::Get().PlaceFromCamera);
            CtxEntry.Owner = GCameraPlaceOwner;
            CtxEntry.SetCommandList(GCmdList);                // ✅
            Section.AddEntry(CtxEntry);
        }

		if (UToolMenu* CBMesh = Menus->ExtendMenu("ContentBrowser.AssetContextMenu.SkeletalMesh"))
        {
            FToolMenuSection& Section = CBMesh->AddSection("CameraPlaceCtx", LOCTEXT("CtxSM","CameraPlaceTool(SkeletalMesh)"));
            FToolMenuEntry CtxEntry = FToolMenuEntry::InitMenuEntry(FCameraPlaceCommands::Get().PlaceFromCamera);
            CtxEntry.Owner = GCameraPlaceOwner;
            CtxEntry.SetCommandList(GCmdList);                // ✅
            Section.AddEntry(CtxEntry);
        }
    }));
}

void UnregisterCameraPlaceMenus()
{
    if (GStartupHandle.IsValid())
    {
        UToolMenus::UnRegisterStartupCallback(GStartupHandle);
        GStartupHandle.Reset();
    }

    // ✅ nullptr 금지: 반드시 우리가 만든 Owner로 해제
    UToolMenus::UnregisterOwner(GCameraPlaceOwner);

    GCmdList.Reset();
    FCameraPlaceCommands::Unregister();
}

#undef LOCTEXT_NAMESPACE