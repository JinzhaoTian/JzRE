/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/JzRERuntime.h"

#include "JzRE/Runtime/Core/JzClock.h"
#include "JzRE/Runtime/Core/JzServiceContainer.h"
#include "JzRE/Runtime/Function/ECS/JzComponents.h"
#include "JzRE/Runtime/Function/ECS/JzWindowComponents.h"
#include "JzRE/Runtime/Function/ECS/JzInputComponents.h"

#include "JzRE/Runtime/Function/Rendering/JzDeviceFactory.h"

// Resource factories for JzAssetManager
#include "JzRE/Runtime/Resource/JzShaderAssetFactory.h"
#include "JzRE/Runtime/Resource/JzModelFactory.h"
#include "JzRE/Runtime/Resource/JzMeshFactory.h"
#include "JzRE/Runtime/Resource/JzTextureFactory.h"
#include "JzRE/Runtime/Resource/JzMaterialFactory.h"

JzRE::JzRERuntime::JzRERuntime(const JzRERuntimeSettings &settings) :
    m_settings(settings)
{
    Startup();
}

JzRE::JzRERuntime::~JzRERuntime()
{
    Shutdown();
}

void JzRE::JzRERuntime::Startup()
{
    JzServiceContainer::Init();

    // coordinator.Initialize();

    CreateSubsystems();
    RegisterComponents();
    RegisterSystems();

    InitializeSubsystems();

    PreloadAssets();
}

void JzRE::JzRERuntime::Shutdown()
{
    // isRunning = false;

    // Event shutdownEvent(EventType::EngineShutdown);
    // eventSystem.Dispatch(shutdownEvent);

    // systemManager.StopAllSystems();

    SaveGameState();

    ShutdownSubsystems();

    // coordinator.DestroyAllEntities();
    // coordinator.Shutdown();

    CleanupGlobals();
}

void JzRE::JzRERuntime::CreateSubsystems()
{
    // In order of initialization
    // windowSystem  = CreateWindowSystem();
    // inputSystem   = CreateInputSystem();
    // assetSystem   = CreateAssetSystem();
    // renderSystem  = CreateRenderSystem();
    // physicsSystem = CreatePhysicsSystem();
    // audioSystem   = CreateAudioSystem();
    // scriptSystem  = CreateScriptSystem();
    // aiSystem      = CreateAISystem();
    // uiSystem      = CreateUISystem();

    // Initialize asset manager (new ECS-friendly asset system)
    JzAssetManagerConfig assetConfig;
    assetConfig.maxCacheMemoryMB = 512;
    assetConfig.asyncWorkerCount = 2;
    m_assetManager               = std::make_unique<JzAssetManager>(assetConfig);
    JzServiceContainer::Provide<JzAssetManager>(*m_assetManager);

    // Create window
    JzWindowSettings windowSettings;
    windowSettings.title = m_settings.windowTitle;
    windowSettings.size  = m_settings.windowSize;
    m_window             = std::make_unique<JzWindow>(m_settings.rhiType, windowSettings);
    m_window->MakeCurrentContext();
    m_window->SetAlignCentered();
    JzServiceContainer::Provide<JzWindow>(*m_window);

    // Create device
    m_device = JzDeviceFactory::CreateDevice(m_settings.rhiType);
    JzServiceContainer::Provide<JzDevice>(*m_device);

    // Create input manager
    m_inputManager = std::make_unique<JzInputManager>(*m_window);
    JzServiceContainer::Provide<JzInputManager>(*m_inputManager);

    // Initialize ECS world and systems
    m_world = std::make_unique<JzWorld>();
    JzServiceContainer::Provide<JzWorld>(*m_world);
}

void JzRE::JzRERuntime::RegisterComponents()
{
    // coordinator.RegisterComponent<Transform>();
    // coordinator.RegisterComponent<MeshRenderer>();
    // coordinator.RegisterComponent<RigidBody>();
    // coordinator.RegisterComponent<Health>();
    // coordinator.RegisterComponent<Animation>();
}

void JzRE::JzRERuntime::RegisterSystems()
{
    // 定义系统签名（组件依赖）
    // Signature transformSig;
    // transformSig.set(coordinator.GetComponentType<Transform>());

    // // 注册系统并设置组件依赖
    // coordinator.RegisterSystem<TransformSystem>();
    // coordinator.SetSystemSignature<TransformSystem>(transformSig);

    // // 设置系统间的依赖关系
    // systemManager.AddDependency<PhysicsSystem, TransformSystem>();
    // systemManager.AddDependency<RenderSystem, TransformSystem>();

    // Register systems in execution order by phase:
    // Window system must be registered first to handle window/input events
    m_windowSystem = m_world->RegisterSystem<JzWindowSystem>();
    m_inputSystem  = m_world->RegisterSystem<JzInputSystem>();

    // Event dispatcher runs after window/input so events queued this frame are dispatched this frame
    m_eventDispatcherSystem = m_world->RegisterSystem<JzEventDispatcherSystem>();
    JzServiceContainer::Provide<JzEventDispatcherSystem>(*m_eventDispatcherSystem);

    m_assetLoadingSystem = m_world->RegisterSystem<JzAssetLoadingSystem>();
    m_cameraSystem       = m_world->RegisterSystem<JzCameraSystem>();
    m_lightSystem        = m_world->RegisterSystem<JzLightSystem>();
    m_renderSystem       = m_world->RegisterSystem<JzRenderSystem>();
}

void JzRE::JzRERuntime::InitializeSubsystems()
{
    m_assetManager->Initialize();

    // Register resource factories with asset manager
    m_assetManager->RegisterFactory<JzShaderAsset>(std::make_unique<JzShaderAssetFactory>());
    m_assetManager->RegisterFactory<JzModel>(std::make_unique<JzModelFactory>());
    m_assetManager->RegisterFactory<JzMesh>(std::make_unique<JzMeshFactory>());
    m_assetManager->RegisterFactory<JzTexture>(std::make_unique<JzTextureFactory>());
    m_assetManager->RegisterFactory<JzMaterial>(std::make_unique<JzMaterialFactory>());

    // Add search paths for asset manager
    auto enginePath = std::filesystem::current_path();
    m_assetManager->AddSearchPath(enginePath.string());
    m_assetManager->AddSearchPath((enginePath / "resources").string());
    m_assetManager->AddSearchPath((enginePath / "resources" / "models").string());
    m_assetManager->AddSearchPath((enginePath / "resources" / "textures").string());
    m_assetManager->AddSearchPath((enginePath / "resources" / "shaders").string());
}

void JzRE::JzRERuntime::PreloadAssets()
{
    // Create default entities
    CreateGlobalConfigEntity();
    CreateDefaultCameraEntity();
    CreateDefaultLightEntity();
}

void JzRE::JzRERuntime::OnFrameBegin()
{
    // TODO: Called at the beginning of each frame
}

void JzRE::JzRERuntime::UpdateSystems(F32 deltaTime)
{
    // plan A
    // UpdateInput();
    // UpdateScripts();
    // UpdateAI();
    // UpdatePhysics();
    // UpdateAnimations();
    // UpdateAudio();
    // UpdateRender();

    // plan B
    // systemManager.ExecuteSystems(deltaTime);

    // Update global window component
    if (m_world->IsValid(m_globalConfigEntity)) {
        auto &config     = m_world->GetComponent<JzWindowComponent>(m_globalConfigEntity);
        config.frameSize = m_window->GetFramebufferSize();
        if (config.frameSize.y > 0) {
            config.aspectRatio = static_cast<F32>(config.frameSize.x) / static_cast<F32>(config.frameSize.y);
        }
    }

    // Update all systems (camera matrices, light collection, culling)
    m_world->Update(deltaTime);
}

void JzRE::JzRERuntime::SynchronizeSystems()
{
    // Synchronize ECS systems (e.g., physics, animation) with the main thread
}

void JzRE::JzRERuntime::OnFrameEnd()
{
    // Called at the end of each frame
}

void JzRE::JzRERuntime::SaveGameState()
{
    // TODO: Save current game state to disk
}

void JzRE::JzRERuntime::ShutdownSubsystems()
{
    // Shutdown all subsystems in reverse order of initialization
    m_renderSystem.reset();
    m_lightSystem.reset();
    m_cameraSystem.reset();
    m_assetLoadingSystem.reset();
    m_eventDispatcherSystem.reset();
    m_inputSystem.reset();
    m_windowSystem.reset();
    m_world.reset();
    m_inputManager.reset();
    m_device.reset();
    m_window.reset();

    // Shutdown asset manager
    if (m_assetManager) {
        m_assetManager->Shutdown();
        m_assetManager.reset();
    }
}

void JzRE::JzRERuntime::CleanupGlobals()
{
    // TODO: Cleanup global entities and components
}

void JzRE::JzRERuntime::CreateGlobalConfigEntity()
{
    // Create Global Config Entity (legacy)
    m_globalConfigEntity = m_world->CreateEntity();

    auto &config        = m_world->AddComponent<JzWindowComponent>(m_globalConfigEntity);
    config.blitToScreen = true;
    config.frameSize    = m_window->GetFramebufferSize();
    if (config.frameSize.y > 0) {
        config.aspectRatio = static_cast<F32>(config.frameSize.x) / static_cast<F32>(config.frameSize.y);
    }

    // Create Window Entity with new ECS components
    m_windowEntity = m_world->CreateEntity();

    // Add enhanced window state component
    auto &windowState           = m_world->AddComponent<JzWindowStateComponent>(m_windowEntity);
    windowState.title           = m_window->GetTitle();
    windowState.size            = m_window->GetSize();
    windowState.position        = m_window->GetPosition();
    windowState.framebufferSize = m_window->GetFramebufferSize();
    windowState.focused         = m_window->IsFocused();
    windowState.visible         = m_window->IsVisible();
    windowState.nativeHandle    = m_window->GetNativeWindow();

    // Add input state component for ECS-based input
    m_world->AddComponent<JzInputStateComponent>(m_windowEntity);

    // Add window event queue component
    m_world->AddComponent<JzWindowEventQueueComponent>(m_windowEntity);

    // Mark as primary window
    m_world->AddComponent<JzPrimaryWindowTag>(m_windowEntity);

    // Set the primary window in WindowSystem
    if (m_windowSystem) {
        m_windowSystem->SetPrimaryWindow(m_windowEntity);
    }
}

void JzRE::JzRERuntime::CreateDefaultCameraEntity()
{
    m_mainCameraEntity = m_world->CreateEntity();

    // Add camera component
    auto &camera        = m_world->AddComponent<JzCameraComponent>(m_mainCameraEntity);
    camera.isMainCamera = true;
    camera.fov          = 60.0f;
    camera.nearPlane    = 0.1f;
    camera.farPlane     = 100.0f;

    // Add orbit controller component
    auto &orbit    = m_world->AddComponent<JzOrbitControllerComponent>(m_mainCameraEntity);
    orbit.distance = 5.0f;
    orbit.pitch    = 0.3f;
    orbit.yaw      = 0.0f;
    orbit.target   = JzVec3(0.0f, 0.0f, 0.0f);

    // Add input components for camera control
    m_world->AddComponent<JzMouseInputComponent>(m_mainCameraEntity);
    m_world->AddComponent<JzKeyboardInputComponent>(m_mainCameraEntity);
    m_world->AddComponent<JzCameraInputComponent>(m_mainCameraEntity);

    // Add main camera tag (empty struct, use emplace directly)
    m_world->AddComponent<JzMainCameraTag>(m_mainCameraEntity);
}

void JzRE::JzRERuntime::CreateDefaultLightEntity()
{
    JzEntity lightEntity = m_world->CreateEntity();

    // Add transform component (not strictly needed for directional light but for consistency)
    m_world->AddComponent<JzTransformComponent>(lightEntity);

    // Add directional light component
    auto &light     = m_world->AddComponent<JzDirectionalLightComponent>(lightEntity);
    light.direction = JzVec3(0.3f, -1.0f, -0.5f);
    light.color     = JzVec3(1.0f, 1.0f, 1.0f);
    light.intensity = 1.0f;
}

void JzRE::JzRERuntime::Run()
{
    OnStart();

    JzRE::JzClock clock;

    while (IsRunning()) {
        auto deltaTime = clock.GetDeltaTime();

        m_window->PollEvents();

        OnFrameBegin();

        // Call user update logic
        OnUpdate(deltaTime);

        UpdateSystems(deltaTime);

        SynchronizeSystems();

        // Call render hook for additional rendering (e.g., ImGui UI)
        OnRender(deltaTime);

        OnFrameEnd();

        // Clear input events for next frame
        m_inputManager->ClearEvents();

        // Clear ECS input state per-frame flags
        if (m_inputSystem) {
            m_inputSystem->ClearFrameState(*m_world);
        }

        // Swap buffers
        m_window->SwapBuffers();

        // Update clock for next frame
        clock.Update();
    }

    OnStop();
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

JzRE::JzWorld &JzRE::JzRERuntime::GetWorld()
{
    return *m_world;
}

JzRE::JzAssetManager &JzRE::JzRERuntime::GetAssetManager()
{
    return *m_assetManager;
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
