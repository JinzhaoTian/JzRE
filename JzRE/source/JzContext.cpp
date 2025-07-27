#include "JzContext.h"

JzRE::JzContext::JzContext() :
    sceneManager()
{
    /* Device */
    JzDeviceSettings deviceSettings;
    deviceSettings.contextVersionMajor = 4;
    deviceSettings.contextVersionMinor = 3;

    device = std::make_unique<JzRE::JzDevice>(deviceSettings);

    /* Window */
    windowSettings.x      = 100;
    windowSettings.y      = 100;
    windowSettings.width  = 1280;
    windowSettings.height = 720;

    window = std::make_unique<JzRE::JzWindow>(*device, windowSettings);

    /* Input Manager */
    inputManager = std::make_unique<JzRE::JzInputManager>(*window);

    window->MakeCurrentContext();

    device->SetVsync(true);

    /* UI Manager */
    uiManager = std::make_unique<JzRE::JzUIManager>(window->GetGLFWWindow());
    uiManager->SetDocking(true);
}

JzRE::JzContext::~JzContext() { }
