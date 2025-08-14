#include "JzHub.h"

JzRE::JzHub::JzHub()
{
    SetupContext();
}

std::optional<std::filesystem::path> JzRE::JzHub::Run()
{
    JzHubPanel panel;

    m_uiManager->SetCanvas(m_canvas);
    m_canvas.AddPanel(panel);

    while (!m_window->ShouldClose()) {
        m_window->PollEvents();
        m_uiManager->Render();
        m_window->SwapBuffers();

        if (!panel.IsOpened()) {
            m_window->SetShouldClose(true);
        }
    }

    return panel.GetResult();
}

void JzRE::JzHub::SetupContext()
{
    auto rhiType = JzERHIType::OpenGL;

    /* Settings */
    JzWindowSettings windowSettings;
    windowSettings.title       = "JzRE Hub";
    windowSettings.width       = 1000;
    windowSettings.height      = 580;
    windowSettings.isMaximized = false;
    windowSettings.isResizable = false;
    windowSettings.isDecorated = true;

    /* Window creation */
    m_window = std::make_unique<JzRE::JzWindow>(rhiType, windowSettings);
    m_window->MakeCurrentContext();

    /* Device */
    m_device = JzRHIFactory::CreateDevice(rhiType);

    m_uiManager = std::make_unique<JzUIManager>(m_window->GetGLFWWindow());
    m_uiManager->SetDocking(true);
}
