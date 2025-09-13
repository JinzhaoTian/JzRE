/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzContext.h"
#include "JzRHIFactory.h"
#include "JzSceneManager.h"

JzRE::JzContext &JzRE::JzContext::GetInstance()
{
    static JzContext instance;
    return instance;
}

JzRE::Bool JzRE::JzContext::Initialize(JzERHIType rhiType)
{
    m_workspacePath = std::filesystem::current_path();

    /* Window */
    m_window = std::make_unique<JzRE::JzWindow>(rhiType, m_windowSettings);

    m_window->MakeCurrentContext();

    m_window->SetFullscreen(true);

    /* Device */
    auto devicePtr = JzRHIFactory::CreateDevice(rhiType);
    if (!devicePtr) {
        return false;
    }

    m_device = std::move(devicePtr);

    m_commandQueue = std::make_unique<JzRHICommandQueue>();

    m_sceneManager = std::make_unique<JzRE::JzSceneManager>();

    /* Input Manager */
    m_inputManager = std::make_unique<JzRE::JzInputManager>(*m_window);

    /* UI Manager */
    m_uiManager = std::make_unique<JzRE::JzUIManager>(m_window->GetGLFWWindow());

    const auto fontPath = m_workspacePath / "assets" / "fonts" / "Roboto-Regular.ttf";

    m_uiManager->LoadFont("roboto-regular-22", fontPath.string(), 22);
    m_uiManager->LoadFont("roboto-regular-18", fontPath.string(), 18);
    m_uiManager->LoadFont("roboto-regular-14", fontPath.string(), 14);
    m_uiManager->UseFont("roboto-regular-18");

    m_uiManager->SetDocking(true);

    return true;
}

JzRE::Bool JzRE::JzContext::IsInitialized() const
{
    return m_device != nullptr;
}

void JzRE::JzContext::Shutdown()
{
    if (m_uiManager) {
        m_uiManager.reset();
    }

    if (m_inputManager) {
        m_inputManager.reset();
    }

    if (m_sceneManager) {
        m_sceneManager.reset();
    }

    if (m_commandQueue) {
        m_commandQueue.reset();
    }

    if (m_device) {
        m_device.reset();
    }

    if (m_window) {
        m_window.reset();
    }
}

JzRE::JzERHIType JzRE::JzContext::GetRHIType() const
{
    return m_device ? m_device->GetRHIType() : JzERHIType::Unknown;
}

JzRE::JzRHIDevice &JzRE::JzContext::GetDevice() const
{
    return *m_device;
}

JzRE::JzWindow &JzRE::JzContext::GetWindow() const
{
    return *m_window;
}

JzRE::JzInputManager &JzRE::JzContext::GetInputManager() const
{
    return *m_inputManager;
}

JzRE::JzUIManager &JzRE::JzContext::GetUIManager() const
{
    return *m_uiManager;
}

JzRE::JzSceneManager &JzRE::JzContext::GetSceneManager() const
{
    return *m_sceneManager;
}

JzRE::JzRHICommandQueue &JzRE::JzContext::GetCommandQueue() const
{
    return *m_commandQueue;
}

void JzRE::JzContext::SetThreadCount(JzRE::U32 threadCount)
{
    if (m_commandQueue) {
        m_commandQueue->SetThreadCount(threadCount);
    }
}

JzRE::U32 JzRE::JzContext::GetThreadCount() const
{
    if (m_commandQueue) {
        return m_commandQueue->GetThreadCount();
    }
    return 1;
}