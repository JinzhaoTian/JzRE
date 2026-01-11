/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/App/JzREInstance.h"
#include "JzRE/Runtime/Core/JzClock.h"
#include "JzRE/Runtime/Core/JzServiceContainer.h"
#include "JzRE/Runtime/Resource/JzTexture.h"
#include "JzRE/Runtime/Resource/JzTextureFactory.h"
#include "JzRE/Runtime/Function/Rendering/JzDeviceFactory.h"
#include "JzRE/Editor/JzContext.h"

JzRE::JzREInstance::JzREInstance(JzERHIType rhiType, std::filesystem::path &openDirectory)
{
    JzServiceContainer::Init();

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

    m_window = std::make_unique<JzWindow>(rhiType, windowSettings);
    m_window->MakeCurrentContext();
    m_window->SetAlignCentered();
    JzServiceContainer::Provide<JzWindow>(*m_window);

    m_device = JzDeviceFactory::CreateDevice(rhiType);
    JzServiceContainer::Provide<JzDevice>(*m_device);

    m_inputManager = std::make_unique<JzInputManager>(*m_window);
    JzServiceContainer::Provide<JzInputManager>(*m_inputManager);

    m_editor = std::make_unique<JzEditor>(*m_window);

    // Create renderer and scene
    m_renderer = std::make_unique<JzRHIRenderer>();
    m_scene    = std::make_shared<JzScene>();
    JzServiceContainer::Provide<JzRHIRenderer>(*m_renderer);
    JzServiceContainer::Provide<JzScene>(*m_scene);

    // Initialize renderer
    m_frameData.frameSize = m_window->GetSize();
    m_renderer->SetFrameSize(m_frameData.frameSize);
    m_renderer->Initialize();

    // Start background worker thread for non-GPU tasks
    // The actual OpenGL rendering stays on the main thread
    m_renderThreadRunning = true;
    m_renderThread        = std::thread(&JzREInstance::_RenderThread, this);
}

JzRE::JzREInstance::~JzREInstance()
{
    // Signal worker thread to stop
    m_renderThreadRunning = false;

    // Wake up the worker thread if it's waiting
    {
        std::lock_guard<std::mutex> lock(m_renderMutex);
        m_frameReady = true;
    }
    m_renderCondition.notify_all();

    // Wait for worker thread to finish
    if (m_renderThread.joinable()) {
        m_renderThread.join();
    }

    // Clean up in reverse order of creation
    m_scene.reset();
    m_renderer.reset();

    if (m_editor) {
        m_editor.reset();
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
        // Handle window events
        m_window->PollEvents();

        // Update frame data
        JzFrameData frameData;
        frameData.deltaTime = clock.GetDeltaTime();
        frameData.frameSize = m_window->GetSize();

        // Signal worker thread for background processing
        _SignalRenderFrame(frameData);

        // Update renderer frame size if changed
        if (frameData.frameSize != m_renderer->GetCurrentFrameSize()) {
            m_renderer->SetFrameSize(frameData.frameSize);
        }

        // Begin frame rendering
        m_renderer->BeginFrame();

        // Render 3D scene to framebuffer (camera is now updated)
        m_renderer->RenderScene(m_scene.get());

        // End scene rendering
        m_renderer->EndFrame();

        // Update and render editor UI (ImGui panels)
        m_editor->Update(clock.GetDeltaTime());

        // Swap buffers
        m_window->SwapBuffers();

        // Clear input events
        m_inputManager->ClearEvents();

        // Wait for worker thread to complete background processing
        _WaitForRenderComplete();

        // Update clock
        clock.Update();
    }
}

void JzRE::JzREInstance::_RenderThread()
{
    // This thread handles non-GPU tasks:
    // - Scene culling
    // - Animation updates
    // - Physics simulation
    // - Asset loading preparation
    //
    // Actual OpenGL rendering stays on the main thread

    JzRE::JzClock workerClock;

    while (m_renderThreadRunning) {
        // Wait for main thread to signal a new frame
        {
            std::unique_lock<std::mutex> lock(m_renderMutex);
            m_renderCondition.wait(lock, [this] {
                return m_frameReady || !m_renderThreadRunning;
            });

            if (!m_renderThreadRunning) {
                break;
            }

            m_frameReady = false;
        }

        // Get frame data safely
        JzFrameData currentFrameData;
        {
            std::lock_guard<std::mutex> lock(m_renderMutex);
            currentFrameData = m_frameData;
        }

        // Perform background processing (non-GPU tasks)
        // TODO: Add scene culling, animation updates, etc.
        // Example:
        // - m_scene->UpdateAnimations(currentFrameData.deltaTime);
        // - m_scene->PerformCulling(camera);
        // - m_resourceManager->ProcessLoadingQueue();

        // Signal main thread that background processing is complete
        {
            std::lock_guard<std::mutex> lock(m_renderMutex);
            m_renderComplete = true;
        }
        m_renderCompleteCondition.notify_one();

        workerClock.Update();
    }
}

void JzRE::JzREInstance::_SignalRenderFrame(const JzFrameData &frameData)
{
    {
        std::lock_guard<std::mutex> lock(m_renderMutex);
        m_frameData      = frameData;
        m_frameReady     = true;
        m_renderComplete = false;
    }
    m_renderCondition.notify_one();
}

void JzRE::JzREInstance::_WaitForRenderComplete()
{
    std::unique_lock<std::mutex> lock(m_renderMutex);
    m_renderCompleteCondition.wait(lock, [this] {
        return m_renderComplete.load();
    });
}

JzRE::Bool JzRE::JzREInstance::IsRunning() const
{
    return !m_window->ShouldClose();
}
