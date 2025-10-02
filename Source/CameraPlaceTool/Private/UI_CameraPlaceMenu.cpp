#include "CameraPlaceSubsystem.h"
#include "ToolMenus.h"
#include "LevelEditor.h"
#include "Framework/Commands/InputChord.h"
#include "Framework/Commands/UICommandList.h"
#include "Framework/Commands/Commands.h"
#define LOCTEXT_NAMESPACE "CameraPlaceTool"   
class FCameraPlaceCommands : public TCommands<FCameraPlaceCommands>
{
public:
    FCameraPlaceCommands()
    : TCommands<FCameraPlaceCommands>(TEXT("CameraPlaceTool"), NSLOCTEXT("CameraPlace", "CameraPlace", "CameraPlaceTool"), NAME_None, FAppStyle::GetAppStyleSetName()) {}

    virtual void RegisterCommands() override
    {
        UI_COMMAND(PlaceFromCamera, "Place From Camera", "Place the selected asset from camera forward", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control | EModifierKey::Alt, EKeys::P));
    }

    TSharedPtr<FUICommandInfo> PlaceFromCamera;
};

static TSharedPtr<FUICommandList> CommandList;

static void ExecutePlaceFromCamera()
{
    if (GEditor)
    {
        if (UCameraPlaceSubsystem* Sys = GEditor->GetEditorSubsystem<UCameraPlaceSubsystem>())
        {
            Sys->PlaceSelectedFromCamera();
        }
    }
}

static void RegisterMenus()
{
    UToolMenus* Menus = UToolMenus::Get();

    // 레벨 에디터 툴바 버튼
    if (UToolMenu* Toolbar = Menus->ExtendMenu("LevelEditor.LevelEditorToolBar"))
    {
        FToolMenuSection& Section = Toolbar->AddSection("CameraPlace", NSLOCTEXT("CameraPlace", "Section", "CameraPlace"));
        Section.AddEntry(FToolMenuEntry::InitToolBarButton(FCameraPlaceCommands::Get().PlaceFromCamera));
    }

    // Content Browser 우클릭 메뉴
    if (UToolMenu* CBMenu = Menus->ExtendMenu("ContentBrowser.AssetContextMenu"))
    {
        FToolMenuSection& Section = CBMenu->AddSection("CameraPlaceCtx", NSLOCTEXT("CameraPlace", "Ctx", "CameraPlaceTool"));
        Section.AddMenuEntry(FCameraPlaceCommands::Get().PlaceFromCamera);
    }
}
#undef LOCTEXT_NAMESPACE