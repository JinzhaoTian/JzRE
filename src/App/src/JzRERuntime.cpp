/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/App/JzRERuntime.h"
#include "JzRE/Runtime/Core/JzClock.h"
#include "JzRE/Runtime/Core/JzServiceContainer.h"
#include "JzRE/Runtime/Function/Rendering/JzDeviceFactory.h"
#include "JzRE/Runtime/Resource/JzTexture.h"
#include "JzRE/Runtime/Resource/JzTextureFactory.h"

JzRE::JzRERuntime::JzRERuntime(JzERHIType rhiType, const String &windowTitle,
                               const JzIVec2 &windowSize)
{
    JzServiceContainer::Init();

    // Initialize resource manager
    m_resourceManager = std::make_unique<JzResourceManager>();
    m_resourceManager->RegisterFactory<JzTexture>(std::make_unique<JzTextureFactory>());
    m_resourceManager->AddSearchPath("./icons");
    JzServiceContainer::Provide<JzResourceManager>(*m_resourceManager);

    // Create window
    JzWindowSettings windowSettings;
    windowSettings.title = windowTitle;
    windowSettings.size  = windowSize;

    m_window = std::make_unique<JzWindow>(rhiType, windowSettings);
    m_window->MakeCurrentContext();
    m_window->SetAlignCentered();
    JzServiceContainer::Provide<JzWindow>(*m_window);

    // Create device
    m_device = JzDeviceFactory::CreateDevice(rhiType);
    JzServiceContainer::Provide<JzDevice>(*m_device);

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
    m_workerThreadRunning = true;
    m_workerThread        = std::thread(&JzRERuntime::_WorkerThread, this);
}

JzRE::JzRERuntime::~JzRERuntime()
{
    // Signal worker thread to stop
    m_workerThreadRunning = false;

    // Wake up the worker thread if it's waiting
    {
        std::lock_guard<std::mutex> lock(m_workerMutex);
        m_frameReady = true;
    }
    m_workerCondition.notify_all();

    // Wait for worker thread to finish
    if (m_workerThread.joinable()) {
        m_workerThread.join();
    }

    // Clean up in reverse order of creation
    m_scene.reset();
    m_renderer.reset();
    m_device.reset();
    m_window.reset();
    m_resourceManager.reset();
}

void JzRE::JzRERuntime::Run()
{
    OnStart();

    JzRE::JzClock clock;

    while (IsRunning()) {
        // Handle window events
        m_window->PollEvents();

        // Update frame data
        JzRuntimeFrameData frameData;
        frameData.deltaTime = clock.GetDeltaTime();
        frameData.frameSize = m_window->GetSize();

        // Signal worker thread for background processing
        _SignalWorkerFrame(frameData);

        // Update renderer frame size if changed
        if (frameData.frameSize != m_renderer->GetCurrentFrameSize()) {
            m_renderer->SetFrameSize(frameData.frameSize);
        }

        // Call user update logic
        OnUpdate(frameData.deltaTime);

        // Begin frame rendering
        m_renderer->BeginFrame();

        // Render 3D scene to framebuffer
        m_renderer->RenderScene(m_scene.get());

        // End scene rendering
        m_renderer->EndFrame();

        // Swap buffers
        m_window->SwapBuffers();

        // Wait for worker thread to complete background processing
        _WaitForWorkerComplete();

        // Update clock
        clock.Update();
    }

    OnStop();
}

void JzRE::JzRERuntime::_WorkerThread()
{
    // This thread handles non-GPU tasks:
    // - Scene culling
    // - Animation updates
    // - Physics simulation
    // - Asset loading preparation
    //
    // Actual OpenGL rendering stays on the main thread

    JzRE::JzClock workerClock;

    while (m_workerThreadRunning) {
        // Wait for main thread to signal a new frame
        {
            std::unique_lock<std::mutex> lock(m_workerMutex);
            m_workerCondition.wait(lock, [this] {
                return m_frameReady || !m_workerThreadRunning;
            });

            if (!m_workerThreadRunning) {
                break;
            }

            m_frameReady = false;
        }

        // Get frame data safely
        JzRuntimeFrameData currentFrameData;
        {
            std::lock_guard<std::mutex> lock(m_workerMutex);
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
            std::lock_guard<std::mutex> lock(m_workerMutex);
            m_workerComplete = true;
        }
        m_workerCompleteCondition.notify_one();

        workerClock.Update();
    }
}

void JzRE::JzRERuntime::_SignalWorkerFrame(const JzRuntimeFrameData &frameData)
{
    {
        std::lock_guard<std::mutex> lock(m_workerMutex);
        m_frameData      = frameData;
        m_frameReady     = true;
        m_workerComplete = false;
    }
    m_workerCondition.notify_one();
}

void JzRE::JzRERuntime::_WaitForWorkerComplete()
{
    std::unique_lock<std::mutex> lock(m_workerMutex);
    m_workerCompleteCondition.wait(lock, [this] {
        return m_workerComplete.load();
    });
}

JzRE::Bool JzRE::JzRERuntime::IsRunning() const
{
    return !m_window->ShouldClose();
}

JzRE::JzWindow &JzRE::JzRERuntime::GetWindow()
{
    return *m_window;
}

JzRE::JzDevice &JzRE::JzRERuntime::GetDevice()
{
    return *m_device;
}

JzRE::JzRHIRenderer &JzRE::JzRERuntime::GetRenderer()
{
    return *m_renderer;
}

std::shared_ptr<JzRE::JzScene> JzRE::JzRERuntime::GetScene()
{
    return m_scene;
}

void JzRE::JzRERuntime::OnStart()
{
    // Default implementation does nothing
    // Override in subclass for custom initialization
}

void JzRE::JzRERuntime::OnUpdate([[maybe_unused]] F32 deltaTime)
{
    // Default implementation does nothing
    // Override in subclass for custom update logic
}

void JzRE::JzRERuntime::OnStop()
{
    // Default implementation does nothing
    // Override in subclass for custom cleanup
}
