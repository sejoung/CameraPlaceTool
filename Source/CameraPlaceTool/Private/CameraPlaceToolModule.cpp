#include "Modules/ModuleManager.h"
#include "ToolMenus.h"
#include "UI_CameraPlaceMenu.h"

class FCameraPlaceToolModule : public IModuleInterface
{
public:
    virtual void StartupModule() override
    {
        FCameraPlaceCommands::Register();
        CommandList = MakeShared<FUICommandList>();
        CommandList->MapAction(FCameraPlaceCommands::Get().PlaceFromCamera,
            FExecuteAction::CreateStatic(&ExecutePlaceFromCamera));

        UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateStatic(&RegisterMenus));
    }

    virtual void ShutdownModule() override
    {
        UToolMenus::UnRegisterStartupCallback(this);
        UToolMenus::UnregisterOwner(this);
        FCameraPlaceCommands::Unregister();
        CommandList.Reset();
    }
};

IMPLEMENT_MODULE(FCameraPlaceToolModule, CameraPlaceTool)