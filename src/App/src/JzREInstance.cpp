/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/App/JzREInstance.h"
#include "JzRE/Core/JzClock.h"
#include "JzRE/Core/JzServiceContainer.h"
#include "JzRE/Resource/JzTexture.h"
#include "JzRE/Resource/JzTextureFactory.h"
#include "JzRE/Editor/JzDeviceFactory.h"
#include "JzRE/Editor/JzContext.h"

JzRE::JzREInstance::JzREInstance(JzERHIType rhiType, std::filesystem::path &openDirectory)
{
    JzServiceContainer::Clear();

    m_resourceManager = std::make_unique<JzResourceManager>();
    m_resourceManager->RegisterFactory<JzTexture>(std::make_unique<JzTextureFactory>());
    m_resourceManager->AddSearchPath("./icons");
    JzServiceContainer::Provide<JzResourceManager>(*m_resourceManager);

    auto &context = JzContext::GetInstance();
    if (!context.IsInitialized()) {
        context.Initialize(openDirectory);
    }

    JzWindowSettings windowSettings;
    windowSettings.title = "JzRE";
    windowSettings.size  = {1280, 720};

    m_window = std::make_unique<JzRE::JzWindow>(rhiType, windowSettings);
    m_window->MakeCurrentContext();
    m_window->SetAlignCentered();

    m_device = JzDeviceFactory::CreateDevice(rhiType);
    JzServiceContainer::Provide<JzRHIDevice>(*m_device);

    m_inputManager = std::make_unique<JzInputManager>(*m_window);
    JzServiceContainer::Provide<JzInputManager>(*m_inputManager);

    m_uiManager = std::make_unique<JzUIManager>(*m_window);

    const auto layoutConfigPath = std::filesystem::current_path() / "config" / "layout.ini";
    m_uiManager->ResetLayout(layoutConfigPath.string());
    m_uiManager->SetEditorLayoutSaveFilename("layout.ini");
    m_uiManager->EnableEditorLayoutSave(true);

    const auto fontPath = openDirectory / "fonts" / "SourceHanSansCN-Regular.otf";
    m_uiManager->LoadFont("sourcehansanscn-regular-18", fontPath.string(), 18);
    m_uiManager->LoadFont("sourcehansanscn-regular-16", fontPath.string(), 16);
    m_uiManager->LoadFont("sourcehansanscn-regular-14", fontPath.string(), 14);
    m_uiManager->UseFont("sourcehansanscn-regular-16");
    m_uiManager->SetDocking(true);

    JzServiceContainer::Provide<JzUIManager>(*m_uiManager);

    m_sceneManager = std::make_unique<JzSceneManager>();
    JzServiceContainer::Provide<JzSceneManager>(*m_sceneManager);

    m_editor = std::make_unique<JzEditor>(*m_window);
}

JzRE::JzREInstance::~JzREInstance()
{
    if (m_editor) {
        m_editor.reset();
    }

    if (m_sceneManager) {
        m_sceneManager.reset();
    }

    if (m_uiManager) {
        m_uiManager.reset();
    }

    if (m_inputManager) {
        m_inputManager.reset();
    }

    if (m_device) {
        m_device.reset();
    }

    if (m_window) {
        m_window.reset();
    }

    if (m_resourceManager) {
        m_resourceManager.reset();
    }
}

void JzRE::JzREInstance::Run()
{
    JzRE::JzClock clock;

    while (IsRunning()) {
        m_window->PollEvents();
        m_editor->Update(clock.GetDeltaTime());
        m_uiManager->Render();
        m_window->SwapBuffers();
        m_inputManager->ClearEvents();

        clock.Update();
    }
}

JzRE::Bool JzRE::JzREInstance::IsRunning() const
{
    return !m_window->ShouldClose();
}
