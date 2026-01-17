/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/JzRERuntime.h"

#include <cmath>

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

    // Create input manager
    m_inputManager = std::make_unique<JzInputManager>(*m_window);
    JzServiceContainer::Provide<JzInputManager>(*m_inputManager);

    // Create renderer and scene
    m_renderer = std::make_unique<JzRHIRenderer>();
    m_scene    = std::make_shared<JzScene>();
    JzServiceContainer::Provide<JzRHIRenderer>(*m_renderer);
    JzServiceContainer::Provide<JzScene>(*m_scene);

    // Initialize renderer with framebuffer size (for Retina/HiDPI displays)
    m_frameData.frameSize = m_window->GetFramebufferSize();
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
    m_inputManager.reset();
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

        // Update frame data (use framebuffer size for Retina/HiDPI)
        JzRuntimeFrameData frameData;
        frameData.deltaTime = clock.GetDeltaTime();
        frameData.frameSize = m_window->GetFramebufferSize();

        // Signal worker thread for background processing
        _SignalWorkerFrame(frameData);

        // Update renderer frame size if changed
        if (frameData.frameSize != m_renderer->GetCurrentFrameSize()) {
            m_renderer->SetFrameSize(frameData.frameSize);
        }

        // Handle default input actions (orbit camera controls)
        _HandleDefaultInputActions(frameData.deltaTime);

        // Call user update logic
        OnUpdate(frameData.deltaTime);

        // Begin frame rendering
        m_renderer->BeginFrame();

        // Render 3D scene to framebuffer
        m_renderer->RenderScene(m_scene.get());

        // End scene rendering
        m_renderer->EndFrame();

        // Blit to screen for standalone runtime (if not using ImGui)
        if (ShouldBlitToScreen()) {
            // Use actual framebuffer size for Retina/HiDPI displays
            JzIVec2 fbSize = m_window->GetFramebufferSize();
            m_renderer->BlitToScreen(static_cast<U32>(fbSize.x()),
                                     static_cast<U32>(fbSize.y()));
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

void JzRE::JzRERuntime::_HandleDefaultInputActions([[maybe_unused]] F32 deltaTime)
{
    // Get current mouse position
    JzVec2 currentMousePos = m_inputManager->GetMousePosition();

    // Calculate mouse delta
    F32 deltaX = 0.0f;
    F32 deltaY = 0.0f;
    if (!m_firstMouse) {
        deltaX = currentMousePos.x() - m_lastMousePos.x();
        deltaY = currentMousePos.y() - m_lastMousePos.y();
    }

    // Track button states - use GetMouseButtonState for real-time GLFW state
    Bool leftPressed  = m_inputManager->GetMouseButtonState(JzEInputMouseButton::MOUSE_BUTTON_LEFT) == JzEInputMouseButtonState::MOUSE_DOWN;
    Bool rightPressed = m_inputManager->GetMouseButtonState(JzEInputMouseButton::MOUSE_BUTTON_RIGHT) == JzEInputMouseButtonState::MOUSE_DOWN;

    // Handle left mouse button - Orbit rotation
    if (leftPressed) {
        if (!m_leftMousePressed) {
            // Just started pressing left button
            m_leftMousePressed = true;
            m_firstMouse       = true;
        } else if (!m_firstMouse) {
            // Dragging with left button
            _HandleOrbitRotation(deltaX, deltaY);
        }
    } else {
        m_leftMousePressed = false;
    }

    // Handle right mouse button - Panning
    if (rightPressed) {
        if (!m_rightMousePressed) {
            // Just started pressing right button
            m_rightMousePressed = true;
            m_firstMouse        = true;
        } else if (!m_firstMouse) {
            // Dragging with right button
            _HandlePanning(deltaX, deltaY);
        }
    } else {
        m_rightMousePressed = false;
    }

    // Handle scroll wheel - Zoom
    JzVec2 scroll = m_inputManager->GetMouseScroll();
    if (std::abs(scroll.y()) > 0.001f) {
        _HandleZoom(scroll.y());
    }

    // Update last mouse position
    m_lastMousePos = currentMousePos;
    m_firstMouse   = false;
}

void JzRE::JzRERuntime::_HandleOrbitRotation(F32 deltaX, F32 deltaY)
{
    // Update yaw and pitch based on mouse movement (drag-object style)
    // - Yaw uses -= : drag right → model rotates right → see left side of model
    // - Pitch uses -= : drag down → model rotates down → see top of model
    m_orbitYaw   -= deltaX * m_orbitSensitivity;
    m_orbitPitch -= deltaY * m_orbitSensitivity;

    // Clamp pitch to avoid gimbal lock (between -89 and 89 degrees)
    constexpr F32 maxPitch = 1.55f; // ~89 degrees in radians
    m_orbitPitch           = std::clamp(m_orbitPitch, -maxPitch, maxPitch);

    // Update camera position
    UpdateCameraFromOrbit();
}

void JzRE::JzRERuntime::_HandlePanning(F32 deltaX, F32 deltaY)
{
    // Calculate the right and up vectors in world space based on current orientation
    F32 cosYaw   = std::cos(m_orbitYaw);
    F32 sinYaw   = std::sin(m_orbitYaw);
    F32 cosPitch = std::cos(m_orbitPitch);
    F32 sinPitch = std::sin(m_orbitPitch);

    // Right vector (perpendicular to the view direction in the horizontal plane)
    JzVec3 right(cosYaw, 0.0f, sinYaw);

    // Up vector (perpendicular to both right and forward)
    JzVec3 up(-sinYaw * sinPitch, cosPitch, cosYaw * sinPitch);

    // Calculate pan amount based on distance (further = larger pan)
    F32 panScale = m_orbitDistance * m_panSensitivity;

    // Move the target point
    m_orbitTarget.x() -= right.x() * deltaX * panScale + up.x() * deltaY * panScale;
    m_orbitTarget.y() += up.y() * deltaY * panScale;
    m_orbitTarget.z() -= right.z() * deltaX * panScale + up.z() * deltaY * panScale;

    // Update camera position
    UpdateCameraFromOrbit();
}

void JzRE::JzRERuntime::_HandleZoom(F32 scrollY)
{
    // Adjust orbit distance based on scroll
    m_orbitDistance -= scrollY * m_zoomSensitivity;

    // Clamp distance to valid range
    m_orbitDistance = std::clamp(m_orbitDistance, m_minDistance, m_maxDistance);

    // Update camera position
    UpdateCameraFromOrbit();
}

void JzRE::JzRERuntime::UpdateCameraFromOrbit()
{
    // Calculate camera position using spherical coordinates
    // x = r * cos(pitch) * sin(yaw)
    // y = r * sin(pitch)
    // z = r * cos(pitch) * cos(yaw)
    F32 cosPitch = std::cos(m_orbitPitch);
    F32 sinPitch = std::sin(m_orbitPitch);
    F32 cosYaw   = std::cos(m_orbitYaw);
    F32 sinYaw   = std::sin(m_orbitYaw);

    JzVec3 cameraPos;
    cameraPos.x() = m_orbitTarget.x() + m_orbitDistance * cosPitch * sinYaw;
    cameraPos.y() = m_orbitTarget.y() + m_orbitDistance * sinPitch;
    cameraPos.z() = m_orbitTarget.z() + m_orbitDistance * cosPitch * cosYaw;

    // Get the scene camera and update its transform
    auto *camera = m_scene->FindMainCamera();
    if (camera) {
        camera->SetPosition(cameraPos);

        // Set camera rotation to look at target
        // The rotation is stored as (pitch, yaw, roll, 0) for now
        // TODO: Convert to proper quaternion when quaternion support is added
        //
        // Camera position uses: x = sin(yaw), z = cos(yaw)
        // So at yaw=0, camera is at +Z looking toward target at origin
        // The forward vector formula in renderer: forward.z = -cos(yaw)
        // At yaw=0, forward = (0, 0, -1), pointing toward -Z
        // To make camera look at target (which is at -Z relative to camera),
        // we need to add PI to yaw so the forward vector points toward target
        JzVec4 rotation;
        rotation.x() = -m_orbitPitch;                          // Pitch (rotation around X axis)
        rotation.y() = m_orbitYaw + 3.14159265358979323846f;   // Yaw + PI to face target
        rotation.z() = 0.0f;                                   // Roll
        rotation.w() = 0.0f;                                   // Unused
        camera->SetRotation(rotation);
    }
}
