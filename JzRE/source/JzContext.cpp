#include "JzContext.h"

JzRE::JzContext::JzContext() :
    sceneManager()
{
    /* Device */
    device = std::make_unique<JzRE::JzDevice>(deviceSettings);

    /* Window */
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
