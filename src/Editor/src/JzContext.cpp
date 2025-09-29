/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Editor/JzContext.h"
#include <memory>
#include "JzRE/Core/JzServiceContainer.h"
#include "JzRE/Editor/JzRHIFactory.h"
#include "JzRE/Resource/JzResourceManager.h"
#include "JzRE/Resource/JzTextureFactory.h"
#include "JzRE/Resource/JzTexture.h"

JzRE::JzContext &JzRE::JzContext::GetInstance()
{
    static JzContext instance;
    return instance;
}

JzRE::Bool JzRE::JzContext::Initialize(JzERHIType rhiType, std::filesystem::path &openDirectory)
{
    m_workDirectory = std::filesystem::current_path();
    m_openDirectory = openDirectory;

    JzServiceContainer::Clear();

    m_resourceManager = std::make_unique<JzResourceManager>();
    m_resourceManager->RegisterFactory<JzTexture>(std::make_unique<JzTextureFactory>());
    m_resourceManager->AddSearchPath("./icons");
    JzServiceContainer::Provide<JzResourceManager>(*m_resourceManager);

    m_windowSettings.title = "JzRE";
    m_windowSettings.size  = {1280, 720};

    m_window = std::make_unique<JzRE::JzWindow>(rhiType, m_windowSettings);
    m_window->MakeCurrentContext();
    m_window->SetAlignCentered();

    m_device = JzRHIFactory::CreateDevice(rhiType);

    m_commandQueue = std::make_unique<JzRHICommandQueue>();

    m_sceneManager = std::make_unique<JzSceneManager>();

    m_inputManager = std::make_unique<JzInputManager>(*m_window);

    m_uiManager = std::make_unique<JzUIManager>(*m_window);

    const auto layoutConfigPath = std::filesystem::current_path() / "config" / "layout.ini";
    m_uiManager->ResetLayout(layoutConfigPath.string());
    m_uiManager->SetEditorLayoutSaveFilename("layout.ini");
    m_uiManager->EnableEditorLayoutSave(true);

    const auto fontPath = m_workDirectory / "fonts" / "SourceHanSansCN-Regular.otf";
    m_uiManager->LoadFont("sourcehansanscn-regular-18", fontPath.string(), 18);
    m_uiManager->LoadFont("sourcehansanscn-regular-16", fontPath.string(), 16);
    m_uiManager->LoadFont("sourcehansanscn-regular-14", fontPath.string(), 14);
    m_uiManager->UseFont("sourcehansanscn-regular-16");
    m_uiManager->SetDocking(true);

    JzServiceContainer::Provide<JzRHIDevice>(*m_device);
    JzServiceContainer::Provide<JzInputManager>(*m_inputManager);

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

std::filesystem::path JzRE::JzContext::GetCurrentPath() const
{
    return m_openDirectory;
}

void JzRE::JzContext::SetRenderFrontend(JzRenderFrontend* frontend)
{
    m_renderFrontend = frontend;
}

JzRE::JzRenderFrontend& JzRE::JzContext::GetRenderFrontend() const
{
    return *m_renderFrontend;
}