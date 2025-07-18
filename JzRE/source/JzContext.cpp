#include "JzContext.h"

JzRE::JzContext::JzContext(const String &projectPath, const String &projectName) :
    projectPath(projectPath),
    projectName(projectName),
    sceneManager()
{
    /* Device */
    JzDeviceSettings deviceSettings;
    deviceSettings.contextVersionMajor = 4;
    deviceSettings.contextVersionMinor = 3;

    device = JzRE::CreateUniquePtr<JzRE::JzDevice>(deviceSettings);

    /* Window */
    windowSettings.x      = 100;
    windowSettings.y      = 100;
    windowSettings.width  = 1280;
    windowSettings.height = 720;

    window = JzRE::CreateUniquePtr<JzRE::JzWindow>(*device, windowSettings);

    /* Input Manager */
    inputManager = JzRE::CreateUniquePtr<JzRE::JzInputManager>(*window);

    window->MakeCurrentContext();

    device->SetVsync(true);

    /* UI Manager */
    uiManager = JzRE::CreateUniquePtr<JzRE::JzUIManager>(window->GetGLFWWindow());
    uiManager->SetDocking(true);
}

JzRE::JzContext::~JzContext() { }
