/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/JzRERuntime.h"

#include "JzRE/Runtime/Core/JzClock.h"
#include "JzRE/Runtime/Function/Project/JzProjectConfig.h"
#include "JzRE/Runtime/Core/JzServiceContainer.h"

#include "JzRE/Runtime/Function/ECS/JzTransformComponents.h"
#include "JzRE/Runtime/Function/ECS/JzCameraComponents.h"
#include "JzRE/Runtime/Function/ECS/JzLightComponents.h"
#include "JzRE/Runtime/Function/ECS/JzInputComponents.h"
#include "JzRE/Runtime/Function/ECS/JzWindowComponents.h"

#include "JzRE/Runtime/Platform/RHI/JzGraphicsContext.h"
#include "JzRE/Runtime/Platform/RHI/JzDeviceFactory.h"

// Resource factories for JzAssetManager
#include "JzRE/Runtime/Resource/JzShaderAssetFactory.h"
#include "JzRE/Runtime/Resource/JzModelFactory.h"
#include "JzRE/Runtime/Resource/JzMeshFactory.h"
#include "JzRE/Runtime/Resource/JzTextureFactory.h"
#include "JzRE/Runtime/Resource/JzMaterialFactory.h"

// Asset import/export services
#include "JzRE/Runtime/Function/Asset/JzAssetImporter.h"
#include "JzRE/Runtime/Function/Asset/JzAssetExporter.h"

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

    JzServiceContainer::Provide<JzProjectManager>(m_projectManager);

    // Load project if specified
    if (!m_settings.projectFile.empty()) {
        auto result = m_projectManager.LoadProject(m_settings.projectFile);
        if (result == JzEProjectResult::Success) {
            // Apply project settings
            const auto &config = m_projectManager.GetConfig();

            // Override window title with project name if not explicitly set
            if (m_settings.windowTitle == "JzRE Runtime") {
                m_settings.windowTitle = config.projectName;
            }

            // Apply render API from project config
            if (config.renderAPI != JzERenderAPI::Auto) {
                m_settings.rhiType = (config.renderAPI == JzERenderAPI::Vulkan) ? JzERHIType::Vulkan : JzERHIType::OpenGL;
            }
        }
    }

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

    // Create graphics context (after window is created in RegisterSystems)
    // Note: device creation is deferred until after window system initialization

    // Initialize ECS world first (needed for system registration)
    // Asset system is created as an ECS system in RegisterSystems()
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
    JzServiceContainer::Provide<JzWindowSystem>(*m_windowSystem);

    // Resolve RHI before window creation to avoid API/window-context mismatch.
    auto requestedRhiType = m_settings.rhiType;
    auto selectedRhiType  = requestedRhiType;
    if (selectedRhiType == JzERHIType::Unknown) {
        selectedRhiType = JzDeviceFactory::GetDefaultRHIType();
    }
    if (selectedRhiType == JzERHIType::Vulkan && !JzDeviceFactory::IsVulkanSupported()) {
        JzRE_LOG_WARN("JzRERuntime: Vulkan requested but unavailable, fallback to OpenGL");
        selectedRhiType = JzERHIType::OpenGL;
    }
    if (selectedRhiType == JzERHIType::Unknown) {
        selectedRhiType = JzERHIType::OpenGL;
    }
    m_settings.rhiType = selectedRhiType;

    JzRE_LOG_INFO("JzRERuntime: requested RHI '{}', selected RHI '{}'",
                  JzDeviceFactory::GetRHITypeName(requestedRhiType),
                  JzDeviceFactory::GetRHITypeName(selectedRhiType));

    // Create GLFW window via the window system
    JzWindowConfig windowConfig;
    windowConfig.title     = m_settings.windowTitle;
    windowConfig.width     = m_settings.windowSize.x;
    windowConfig.height    = m_settings.windowSize.y;
    windowConfig.decorated = m_settings.windowDecorated;
    m_windowSystem->InitializeWindow(m_settings.rhiType, windowConfig);
    m_windowSystem->SetAlignCentered();

    // Create graphics context (requires GL context to be current)
    m_graphicsContext   = std::make_unique<JzGraphicsContext>();
    auto *windowBackend = m_windowSystem->GetBackend();
    if (windowBackend) {
        m_graphicsContext->Initialize(*windowBackend, m_settings.rhiType);
        JzServiceContainer::Provide<JzGraphicsContext>(*m_graphicsContext);
        JzServiceContainer::Provide<JzDevice>(m_graphicsContext->GetDevice());
    }

    m_inputSystem = m_world->RegisterSystem<JzInputSystem>();

    // Event dispatcher runs after window/input so events queued this frame are dispatched this frame
    m_eventSystem = m_world->RegisterSystem<JzEventSystem>();
    JzServiceContainer::Provide<JzEventSystem>(*m_eventSystem);
    m_world->SetContext<JzEventSystem *>(m_eventSystem.get());

    m_assetSystem = m_world->RegisterSystem<JzAssetSystem>();
    JzServiceContainer::Provide<JzAssetSystem>(*m_assetSystem);

    m_cameraSystem = m_world->RegisterSystem<JzCameraSystem>();
    m_lightSystem  = m_world->RegisterSystem<JzLightSystem>();
    m_renderSystem = m_world->RegisterSystem<JzRenderSystem>();
    JzServiceContainer::Provide<JzRenderSystem>(*m_renderSystem);
}

void JzRE::JzRERuntime::InitializeSubsystems()
{
    // Initialize asset system (creates and initializes internal JzAssetManager)
    JzAssetManagerConfig assetConfig;
    assetConfig.maxCacheMemoryMB = 512;
    assetConfig.asyncWorkerCount = 2;
    m_assetSystem->Initialize(*m_world, assetConfig);

    // Register resource factories
    m_assetSystem->RegisterFactory<JzShaderAsset>(std::make_unique<JzShaderAssetFactory>());
    m_assetSystem->RegisterFactory<JzModel>(std::make_unique<JzModelFactory>());
    m_assetSystem->RegisterFactory<JzMesh>(std::make_unique<JzMeshFactory>());
    m_assetSystem->RegisterFactory<JzTexture>(std::make_unique<JzTextureFactory>());
    m_assetSystem->RegisterFactory<JzMaterial>(std::make_unique<JzMaterialFactory>());

    // Add search paths based on project or engine defaults
    if (m_projectManager.HasLoadedProject()) {
        const auto &config      = m_projectManager.GetConfig();
        auto        contentPath = config.GetContentPath();

        m_assetSystem->AddSearchPath(config.rootPath.string());
        m_assetSystem->AddSearchPath(contentPath.string());
        m_assetSystem->AddSearchPath((contentPath / "Models").string());
        m_assetSystem->AddSearchPath((contentPath / "Textures").string());
        m_assetSystem->AddSearchPath((contentPath / "Shaders").string());
        m_assetSystem->AddSearchPath((contentPath / "Materials").string());
    } else {
        // Default engine paths when no project is loaded
        auto enginePath = std::filesystem::current_path();
        m_assetSystem->AddSearchPath(enginePath.string());
        m_assetSystem->AddSearchPath((enginePath / "resources").string());
        m_assetSystem->AddSearchPath((enginePath / "resources" / "models").string());
        m_assetSystem->AddSearchPath((enginePath / "resources" / "textures").string());
        m_assetSystem->AddSearchPath((enginePath / "resources" / "shaders").string());
    }

    // Create and register asset import/export services
    m_assetImporter = std::make_unique<JzAssetImporter>();
    m_assetExporter = std::make_unique<JzAssetExporter>();
    JzServiceContainer::Provide<JzAssetImporter>(*m_assetImporter);
    JzServiceContainer::Provide<JzAssetExporter>(*m_assetExporter);
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
    // Shutdown asset import/export services
    JzServiceContainer::Remove<JzAssetImporter>();
    JzServiceContainer::Remove<JzAssetExporter>();
    m_assetImporter.reset();
    m_assetExporter.reset();

    // Ensure system-level shutdown runs before graphics context/device teardown.
    if (m_world) {
        m_world->ShutdownSystems();
    }

    // Shutdown all subsystems in reverse order of initialization
    JzServiceContainer::Remove<JzRenderSystem>();
    JzServiceContainer::Remove<JzAssetSystem>();
    JzServiceContainer::Remove<JzEventSystem>();
    JzServiceContainer::Remove<JzWindowSystem>();
    m_renderSystem.reset();
    m_lightSystem.reset();
    m_cameraSystem.reset();
    m_assetSystem.reset();
    m_eventSystem.reset();
    m_inputSystem.reset();
    if (m_graphicsContext) {
        JzServiceContainer::Remove<JzGraphicsContext>();
        JzServiceContainer::Remove<JzDevice>();
        m_graphicsContext->Shutdown();
        m_graphicsContext.reset();
    }

    m_windowSystem.reset();
    JzServiceContainer::Remove<JzWorld>();
    m_world.reset();
}

void JzRE::JzRERuntime::CleanupGlobals()
{
    // TODO: Cleanup global entities and components
}

void JzRE::JzRERuntime::CreateGlobalConfigEntity()
{
    // Create Window Entity with ECS components
    m_windowEntity = m_world->CreateEntity();

    // Add enhanced window state component
    auto &windowState           = m_world->AddComponent<JzWindowStateComponent>(m_windowEntity);
    windowState.title           = m_windowSystem->GetTitle();
    windowState.size            = m_windowSystem->GetSize();
    windowState.position        = m_windowSystem->GetPosition();
    windowState.framebufferSize = m_windowSystem->GetFramebufferSize();
    windowState.focused         = m_windowSystem->IsFocused();
    windowState.visible         = m_windowSystem->IsVisible();
    windowState.nativeHandle    = m_windowSystem->GetNativeWindow();

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

        m_windowSystem->PollWindowEvents();

        OnFrameBegin();

        // Call user update logic
        OnUpdate(deltaTime);

        if (m_graphicsContext) {
            m_graphicsContext->BeginFrame();
        }

        UpdateSystems(deltaTime);

        SynchronizeSystems();

        // Call render hook for additional rendering (e.g., host UI)
        OnRender(deltaTime);

        if (m_graphicsContext) {
            m_graphicsContext->EndFrame();
        }

        OnFrameEnd();

        // Clear ECS input state per-frame flags
        if (m_inputSystem) {
            m_inputSystem->ClearFrameState(*m_world);
        }

        // Present frame
        if (m_graphicsContext) {
            m_graphicsContext->Present();
        }

        // Update clock for next frame
        clock.Update();
    }

    OnStop();
}

JzRE::Bool JzRE::JzRERuntime::IsRunning() const
{
    return !m_windowSystem->ShouldClose();
}

JzRE::JzWorld &JzRE::JzRERuntime::GetWorld()
{
    return *m_world;
}

JzRE::JzAssetSystem &JzRE::JzRERuntime::GetAssetSystem()
{
    return *m_assetSystem;
}

JzRE::JzProjectManager &JzRE::JzRERuntime::GetProjectManager()
{
    return m_projectManager;
}

JzRE::Bool JzRE::JzRERuntime::HasProject() const
{
    return m_projectManager.HasLoadedProject();
}

const JzRE::JzProjectConfig *JzRE::JzRERuntime::GetProjectConfig() const
{
    if (m_projectManager.HasLoadedProject()) {
        return &m_projectManager.GetConfig();
    }
    return nullptr;
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
    // Override in subclass to render additional content (e.g., host UI)
}

void JzRE::JzRERuntime::OnStop()
{
    // Default implementation does nothing
    // Override in subclass for custom cleanup
}
