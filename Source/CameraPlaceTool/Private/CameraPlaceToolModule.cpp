#include "Modules/ModuleManager.h"
#include "UI_CameraPlaceMenu.h"

class FCameraPlaceToolModule : public IModuleInterface
{
public:
    virtual void StartupModule() override
    {
        RegisterCameraPlaceMenus();
    }
    virtual void ShutdownModule() override
    {
        UnregisterCameraPlaceMenus();
    }
};

IMPLEMENT_MODULE(FCameraPlaceToolModule, CameraPlaceTool)