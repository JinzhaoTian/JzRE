/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/JzRERuntime.h"

#include "JzRE/Runtime/Core/JzClock.h"
#include "JzRE/Runtime/Core/JzServiceContainer.h"
#include "JzRE/Runtime/Function/ECS/JzEnttRenderComponents.h"
#include "JzRE/Runtime/Function/Rendering/JzDeviceFactory.h"
#include "JzRE/Runtime/JzContext.h"

JzRE::JzRERuntime::JzRERuntime(JzERHIType rhiType, const String &windowTitle,
                               const JzIVec2 &windowSize)
{
    JzServiceContainer::Init();

    // Initialize resource manager
    m_resourceManager = std::make_unique<JzResourceManager>();
    JzServiceContainer::Provide<JzResourceManager>(*m_resourceManager);

    // Initialize engine context (registers factories and engine search paths)
    auto &context = JzContext::GetInstance();
    context.InitializeEngine(*m_resourceManager);

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

    // Create input manager
    m_inputManager = std::make_unique<JzInputManager>(*m_window);
    JzServiceContainer::Provide<JzInputManager>(*m_inputManager);

    // Initialize ECS world and systems
    InitializeECS();

    // Provide ECS services for Editor access
    JzServiceContainer::Provide<JzEnttWorld>(*m_world);
    JzServiceContainer::Provide<JzEnttCameraSystem>(*m_cameraSystem);
    JzServiceContainer::Provide<JzEnttRenderSystem>(*m_renderSystem);

    // Initialize frame data with framebuffer size (for Retina/HiDPI displays)
    m_frameData.frameSize = m_window->GetFramebufferSize();
    m_renderSystem->SetFrameSize(m_frameData.frameSize);

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
    m_renderSystem.reset();
    m_lightSystem.reset();
    m_cameraSystem.reset();
    m_world.reset();
    m_inputManager.reset();
    m_device.reset();
    m_window.reset();
    m_resourceManager.reset();
}

void JzRE::JzRERuntime::InitializeECS()
{
    m_world = std::make_unique<JzEnttWorld>();

    // Register systems in update order
    m_cameraSystem = m_world->RegisterSystem<JzEnttCameraSystem>();
    m_lightSystem  = m_world->RegisterSystem<JzEnttLightSystem>();
    m_renderSystem = m_world->RegisterSystem<JzEnttRenderSystem>();

    // Wire up system dependencies
    m_renderSystem->SetCameraSystem(m_cameraSystem);
    m_renderSystem->SetLightSystem(m_lightSystem);

    // Create default entities
    CreateDefaultCameraEntity();
    CreateDefaultLightEntity();
}

void JzRE::JzRERuntime::CreateDefaultCameraEntity()
{
    m_mainCameraEntity = m_world->CreateEntity();

    // Add camera component
    auto &camera        = m_world->AddComponent<JzEnttCameraComponent>(m_mainCameraEntity);
    camera.isMainCamera = true;
    camera.fov          = 60.0f;
    camera.nearPlane    = 0.1f;
    camera.farPlane     = 100.0f;

    // Add orbit controller component
    auto &orbit    = m_world->AddComponent<JzEnttOrbitControllerComponent>(m_mainCameraEntity);
    orbit.distance = 5.0f;
    orbit.pitch    = 0.3f;
    orbit.yaw      = 0.0f;
    orbit.target   = JzVec3(0.0f, 0.0f, 0.0f);

    // Add main camera tag (empty struct, use emplace directly)
    m_world->GetRegistry().emplace<JzMainCameraTag>(m_mainCameraEntity);
}

void JzRE::JzRERuntime::CreateDefaultLightEntity()
{
    JzEnttEntity lightEntity = m_world->CreateEntity();

    // Add transform component (not strictly needed for directional light but for consistency)
    m_world->AddComponent<JzTransformComponent>(lightEntity);

    // Add directional light component
    auto &light     = m_world->AddComponent<JzEnttDirectionalLightComponent>(lightEntity);
    light.direction = JzVec3(0.3f, -1.0f, -0.5f);
    light.color     = JzVec3(1.0f, 1.0f, 1.0f);
    light.intensity = 1.0f;
}

void JzRE::JzRERuntime::Run()
{
    OnStart();

    JzRE::JzClock clock;

    while (IsRunning()) {
        // Handle window events
        m_window->PollEvents();

        // Update frame data (use framebuffer size for Retina/HiDPI)
        JzRuntimeFrameData frameData;
        frameData.deltaTime = clock.GetDeltaTime();
        frameData.frameSize = m_window->GetFramebufferSize();

        // Signal worker thread for background processing
        _SignalWorkerFrame(frameData);

        // Update camera system aspect ratio
        if (frameData.frameSize.x > 0 && frameData.frameSize.y > 0) {
            F32 aspect = static_cast<F32>(frameData.frameSize.x) / static_cast<F32>(frameData.frameSize.y);
            m_cameraSystem->SetAspectRatio(aspect);
        }

        // Call user update logic
        OnUpdate(frameData.deltaTime);

        // Update renderer frame size if changed
        if (frameData.frameSize != m_renderSystem->GetCurrentFrameSize()) {
            m_renderSystem->SetFrameSize(frameData.frameSize);
        }

        // Begin frame rendering
        m_renderSystem->BeginFrame();

        // ECS update: Camera -> Light -> Render (in registration order)
        m_world->Update(frameData.deltaTime);

        // End scene rendering
        m_renderSystem->EndFrame();

        // Blit to screen for standalone runtime (if not using ImGui)
        if (ShouldBlitToScreen()) {
            // Use actual framebuffer size for Retina/HiDPI displays
            JzIVec2 fbSize = m_window->GetFramebufferSize();
            m_renderSystem->BlitToScreen(static_cast<U32>(fbSize.x),
                                         static_cast<U32>(fbSize.y));
        }

        // Call render hook for additional rendering (e.g., ImGui UI)
        OnRender(frameData.deltaTime);

        // Swap buffers
        m_window->SwapBuffers();

        // Clear input events
        m_inputManager->ClearEvents();

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

JzRE::JzEnttWorld &JzRE::JzRERuntime::GetWorld()
{
    return *m_world;
}

std::shared_ptr<JzRE::JzEnttCameraSystem> JzRE::JzRERuntime::GetCameraSystem()
{
    return m_cameraSystem;
}

std::shared_ptr<JzRE::JzEnttLightSystem> JzRE::JzRERuntime::GetLightSystem()
{
    return m_lightSystem;
}

std::shared_ptr<JzRE::JzEnttRenderSystem> JzRE::JzRERuntime::GetRenderSystem()
{
    return m_renderSystem;
}

JzRE::JzInputManager &JzRE::JzRERuntime::GetInputManager()
{
    return *m_inputManager;
}

JzRE::JzResourceManager &JzRE::JzRERuntime::GetResourceManager()
{
    return *m_resourceManager;
}

JzRE::F32 JzRE::JzRERuntime::GetDeltaTime() const
{
    return m_frameData.deltaTime;
}

JzRE::JzEnttEntity JzRE::JzRERuntime::GetMainCameraEntity() const
{
    return m_mainCameraEntity;
}

const JzRE::JzRuntimeFrameData &JzRE::JzRERuntime::GetFrameData() const
{
    return m_frameData;
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

void JzRE::JzRERuntime::OnRender([[maybe_unused]] F32 deltaTime)
{
    // Default implementation does nothing
    // Override in subclass to render additional content (e.g., ImGui UI)
}

void JzRE::JzRERuntime::OnStop()
{
    // Default implementation does nothing
    // Override in subclass for custom cleanup
}

JzRE::Bool JzRE::JzRERuntime::ShouldBlitToScreen() const
{
    // Default: blit to screen for standalone runtime
    // Override and return false in Editor to use ImGui for display
    return true;
}
