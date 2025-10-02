#include "UI_CameraPlaceMenu.h"
#include "CameraPlaceSubsystem.h"
#include "ToolMenus.h"
#include "LevelEditor.h"
#include "Framework/Commands/InputChord.h"
#include "Framework/Commands/UICommandList.h"
#include "Framework/Commands/Commands.h"
#include "Styling/AppStyle.h"

#define LOCTEXT_NAMESPACE "CameraPlaceTool"

class FCameraPlaceCommands : public TCommands<FCameraPlaceCommands>
{
public:
    FCameraPlaceCommands()
    : TCommands<FCameraPlaceCommands>(TEXT("CameraPlaceTool"),
      LOCTEXT("CameraPlace", "CameraPlaceTool"),
      NAME_None, FAppStyle::GetAppStyleSetName()) {}
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
    GCmdList->MapAction(FCameraPlaceCommands::Get().PlaceFromCamera,
        FExecuteAction::CreateStatic(&ExecutePlaceFromCamera));

    UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateLambda([]()
    {
        if (UToolMenu* Toolbar = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar"))
        {
            FToolMenuSection& Section = Toolbar->AddSection("CameraPlace", LOCTEXT("Section","CameraPlace"));
            Section.AddEntry(FToolMenuEntry::InitToolBarButton(FCameraPlaceCommands::Get().PlaceFromCamera));
        }
        if (UToolMenu* CB = UToolMenus::Get()->ExtendMenu("ContentBrowser.AssetContextMenu"))
        {
            FToolMenuSection& Section = CB->AddSection("CameraPlaceCtx", LOCTEXT("Ctx","CameraPlaceTool"));
            Section.AddMenuEntry(FCameraPlaceCommands::Get().PlaceFromCamera);
        }
    }));
}

void UnregisterCameraPlaceMenus()
{
    UToolMenus::UnRegisterStartupCallback(nullptr);
    UToolMenus::UnregisterOwner(nullptr);
    GCmdList.Reset();
    FCameraPlaceCommands::Unregister();
}

#undef LOCTEXT_NAMESPACE