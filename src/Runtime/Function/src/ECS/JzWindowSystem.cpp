/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Function/ECS/JzWindowSystem.h"

#include <stdexcept>
#if defined(_WIN32)
#define GLFW_EXPOSE_NATIVE_WIN32
#elif defined(__APPLE__)
#define GLFW_EXPOSE_NATIVE_COCOA
#elif defined(__linux__)
#define GLFW_EXPOSE_NATIVE_X11
#endif
#include <GLFW/glfw3native.h>

#include "JzRE/Runtime/Core/JzServiceContainer.h"
#include "JzRE/Runtime/Function/Event/JzEventDispatcherSystem.h"
#include "JzRE/Runtime/Function/Event/JzWindowEvents.h"

namespace JzRE {

// Static member initialization
std::unordered_map<GLFWwindow *, JzWindowSystem *> JzWindowSystem::s_windowMap;

// ==================== Lifecycle ====================

JzWindowSystem::~JzWindowSystem()
{
    ReleaseWindow();
}

void JzWindowSystem::OnInit(JzWorld &world)
{
    // Window entity should be created externally (by JzRERuntime) and registered
    // This system just manages the synchronization
}

void JzWindowSystem::Update(JzWorld &world, F32 delta)
{
    if (!m_glfwWindow) return;

    // Poll events from backend
    PollEvents(world);

    // Update window state components from backend
    UpdateWindowState(world);

    // Update input state from window callbacks
    UpdateInputState(world);

    // Process window events
    ProcessWindowEvents(world);

    // Emit typed ECS events through the event dispatcher
    EmitWindowEvents(world);

    // Apply any component changes to backend
    ApplyComponentChanges(world);

    // Handle close requests
    HandleCloseRequests(world);

    // Update statistics
    UpdateStatistics(world, delta);
}

void JzWindowSystem::OnShutdown(JzWorld &world)
{
    // Window destruction is handled by the destructor or explicit ReleaseWindow() call
}

// ==================== Window Creation ====================

void JzWindowSystem::InitializeWindow(JzERHIType rhiType, const JzWindowConfig &config)
{
    m_rhiType      = rhiType;
    m_title        = config.title;
    m_size         = JzIVec2(config.width, config.height);
    m_windowedSize = m_size;
    m_fullscreen   = config.fullscreen;

    CreateGlfwWindow(config);
    SetupCallbacks();

    WindowResizedEvent.Add(std::bind(&JzWindowSystem::OnResize, this, std::placeholders::_1));
    WindowMoveEvent.Add(std::bind(&JzWindowSystem::OnMove, this, std::placeholders::_1));
}

JzEntity JzWindowSystem::CreateWindowEntity(JzWorld &world, const JzWindowConfig &config)
{
    JzEntity entity = world.CreateEntity();

    // Add window state component
    auto &windowState           = world.AddComponent<JzWindowStateComponent>(entity);
    windowState.title           = config.title;
    windowState.size            = JzIVec2(config.width, config.height);
    windowState.framebufferSize = windowState.size;
    windowState.resizable       = config.resizable;
    windowState.decorated       = config.decorated;
    windowState.floating        = config.floating;
    windowState.visible         = config.visible;
    windowState.swapInterval    = config.vsync ? 1 : 0;

    if (config.fullscreen) {
        windowState.state = JzEWindowState::Fullscreen;
    }

    // Add event queue component
    world.AddComponent<JzWindowEventQueueComponent>(entity);

    // Add input state component for this window
    world.AddComponent<JzInputStateComponent>(entity);

    return entity;
}

void JzWindowSystem::ReleaseWindow()
{
    if (m_glfwWindow) {
        s_windowMap.erase(m_glfwWindow);
        glfwDestroyWindow(m_glfwWindow);
        m_glfwWindow = nullptr;
    }
}

// ==================== Window Operations ====================

void JzWindowSystem::MakeCurrentContext() const
{
    if (m_glfwWindow) {
        glfwMakeContextCurrent(m_glfwWindow);
    }
}

void JzWindowSystem::DetachContext() const
{
    glfwMakeContextCurrent(nullptr);
}

void JzWindowSystem::PollWindowEvents() const
{
    glfwPollEvents();
}

void JzWindowSystem::SwapWindowBuffers() const
{
    if (m_glfwWindow) {
        glfwSwapBuffers(m_glfwWindow);
    }
}

// ==================== Window Properties ====================

GLFWwindow *JzWindowSystem::GetGLFWWindow() const
{
    return m_glfwWindow;
}

void *JzWindowSystem::GetNativeWindow() const
{
    if (!m_glfwWindow) return nullptr;

#if defined(_WIN32)
    return glfwGetWin32Window(m_glfwWindow);
#elif defined(__APPLE__)
    return glfwGetCocoaWindow(m_glfwWindow);
#elif defined(__linux__)
    return reinterpret_cast<void *>(glfwGetX11Window(m_glfwWindow));
#else
    return nullptr;
#endif
}

void JzWindowSystem::SetTitle(const String &title)
{
    m_title = title;
    if (m_glfwWindow) {
        glfwSetWindowTitle(m_glfwWindow, title.c_str());
    }
}

String JzWindowSystem::GetTitle() const
{
    return m_title;
}

void JzWindowSystem::SetPosition(JzIVec2 position)
{
    if (!m_fullscreen && m_glfwWindow) {
        glfwSetWindowPos(m_glfwWindow, position.x, position.y);
        m_position = position;
    }
}

JzIVec2 JzWindowSystem::GetPosition() const
{
    if (!m_glfwWindow) return m_position;
    I32 x, y;
    glfwGetWindowPos(m_glfwWindow, &x, &y);
    return {x, y};
}

void JzWindowSystem::SetSize(JzIVec2 size)
{
    if (!m_fullscreen && m_glfwWindow) {
        glfwSetWindowSize(m_glfwWindow, size.x, size.y);
        m_size = size;
    }
}

JzIVec2 JzWindowSystem::GetSize() const
{
    if (!m_glfwWindow) return m_size;
    I32 width, height;
    glfwGetWindowSize(m_glfwWindow, &width, &height);
    return {width, height};
}

JzIVec2 JzWindowSystem::GetFramebufferSize() const
{
    if (!m_glfwWindow) return m_size;
    I32 width, height;
    glfwGetFramebufferSize(m_glfwWindow, &width, &height);
    return {width, height};
}

JzIVec2 JzWindowSystem::GetMonitorSize() const
{
    const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    return {static_cast<I32>(mode->width), static_cast<I32>(mode->height)};
}

void JzWindowSystem::SetMinimumSize(JzIVec2 minimumSize)
{
    m_minimumSize = minimumSize;
    UpdateSizeLimit();
}

JzIVec2 JzWindowSystem::GetMinimumSize() const
{
    return m_minimumSize;
}

void JzWindowSystem::SetMaximumSize(JzIVec2 maximumSize)
{
    m_maximumSize = maximumSize;
    UpdateSizeLimit();
}

JzIVec2 JzWindowSystem::GetMaximumSize() const
{
    return m_maximumSize;
}

// ==================== Window State ====================

void JzWindowSystem::SetFullscreen(Bool value)
{
    if (!m_glfwWindow) return;

    if (value) {
        m_fullscreen   = true;
        m_windowedSize = m_size;
        m_windowedPos  = m_position;

        glfwSetWindowMonitor(
            m_glfwWindow,
            glfwGetPrimaryMonitor(),
            static_cast<I32>(m_position.x),
            static_cast<I32>(m_position.y),
            static_cast<I32>(m_size.x),
            static_cast<I32>(m_size.y),
            m_refreshRate);
    } else {
        m_fullscreen = false;
        m_size       = m_windowedSize;
        m_position   = m_windowedPos;

        glfwSetWindowMonitor(
            m_glfwWindow,
            nullptr,
            static_cast<I32>(m_position.x),
            static_cast<I32>(m_position.y),
            static_cast<I32>(m_size.x),
            static_cast<I32>(m_size.y),
            m_refreshRate);
    }
}

Bool JzWindowSystem::IsFullscreen() const
{
    if (!m_glfwWindow) return false;
    return glfwGetWindowMonitor(m_glfwWindow) != nullptr;
}

void JzWindowSystem::SetAlignCentered()
{
    if (!m_glfwWindow) return;

    const GLFWvidmode *mode      = glfwGetVideoMode(glfwGetPrimaryMonitor());
    auto               monWidth  = mode->width;
    auto               monHeight = mode->height;

    I32 winWidth, winHeight;
    glfwGetWindowSize(m_glfwWindow, &winWidth, &winHeight);

    glfwSetWindowPos(m_glfwWindow, monWidth / 2 - winWidth / 2, monHeight / 2 - winHeight / 2);
}

Bool JzWindowSystem::IsMinimized() const
{
    if (!m_glfwWindow) return false;
    return glfwGetWindowAttrib(m_glfwWindow, GLFW_ICONIFIED) == GLFW_TRUE;
}

void JzWindowSystem::Minimize() const
{
    if (m_glfwWindow) {
        glfwIconifyWindow(m_glfwWindow);
    }
}

void JzWindowSystem::Restore() const
{
    if (m_glfwWindow) {
        glfwRestoreWindow(m_glfwWindow);
    }
}

Bool JzWindowSystem::IsMaximized() const
{
    if (!m_glfwWindow) return false;
    return glfwGetWindowAttrib(m_glfwWindow, GLFW_MAXIMIZED) == GLFW_TRUE;
}

void JzWindowSystem::Maximize() const
{
    if (m_glfwWindow) {
        glfwMaximizeWindow(m_glfwWindow);
    }
}

Bool JzWindowSystem::IsHidden() const
{
    if (!m_glfwWindow) return false;
    return glfwGetWindowAttrib(m_glfwWindow, GLFW_VISIBLE) == GLFW_FALSE;
}

Bool JzWindowSystem::IsVisible() const
{
    if (!m_glfwWindow) return false;
    return glfwGetWindowAttrib(m_glfwWindow, GLFW_VISIBLE) == GLFW_TRUE;
}

void JzWindowSystem::Hide() const
{
    if (m_glfwWindow) {
        glfwHideWindow(m_glfwWindow);
    }
}

void JzWindowSystem::Show() const
{
    if (m_glfwWindow) {
        glfwShowWindow(m_glfwWindow);
    }
}

Bool JzWindowSystem::IsFocused() const
{
    if (!m_glfwWindow) return false;
    return glfwGetWindowAttrib(m_glfwWindow, GLFW_FOCUSED) == GLFW_TRUE;
}

void JzWindowSystem::Focus() const
{
    if (m_glfwWindow) {
        glfwFocusWindow(m_glfwWindow);
    }
}

void JzWindowSystem::SetShouldClose(Bool value) const
{
    if (m_glfwWindow) {
        glfwSetWindowShouldClose(m_glfwWindow, value);
    }
}

Bool JzWindowSystem::ShouldClose() const
{
    if (!m_glfwWindow) return true;
    return glfwWindowShouldClose(m_glfwWindow);
}

// ==================== GLFW Window Creation (Private) ====================

void JzWindowSystem::CreateGlfwWindow(const JzWindowConfig &config)
{
    I32 initCode = glfwInit();
    if (initCode == GLFW_FALSE) {
        glfwTerminate();
        throw std::runtime_error("Failed to Init GLFW");
    }

    glfwDefaultWindowHints();

    GLFWmonitor *selectedMonitor = nullptr;
    if (m_fullscreen) {
        selectedMonitor = glfwGetPrimaryMonitor();
    }

    glfwWindowHint(GLFW_RESIZABLE, config.resizable);
    glfwWindowHint(GLFW_DECORATED, config.decorated);
    glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_FALSE);
    glfwWindowHint(GLFW_FLOATING, config.floating);
    glfwWindowHint(GLFW_VISIBLE, config.visible);
    glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE);
    glfwWindowHint(GLFW_SAMPLES, config.samples);

    switch (m_rhiType) {
        case JzERHIType::Unknown:
            break;

        case JzERHIType::OpenGL:
            glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
            break;

        case JzERHIType::Vulkan:
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            break;

        case JzERHIType::D3D11:
            break;

        case JzERHIType::D3D12:
            break;

        case JzERHIType::Metal:
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#ifdef __APPLE__
            glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
#endif
            break;
    }

    m_glfwWindow = glfwCreateWindow(
        m_size.x,
        m_size.y,
        m_title.c_str(),
        selectedMonitor,
        nullptr);

    if (!m_glfwWindow) {
        const char *description;
        I32         error_code    = glfwGetError(&description);
        String      error_message = "Failed to create GLFW window";
        if (error_code != GLFW_NO_ERROR) {
            error_message += " - GLFW Error " + std::to_string(error_code) + ": " + std::string(description);
        }
        throw std::runtime_error(error_message);
    }

    glfwSetWindowSizeLimits(
        m_glfwWindow,
        m_minimumSize.x,
        m_minimumSize.y,
        m_maximumSize.x,
        m_maximumSize.y);

    glfwSetWindowPos(
        m_glfwWindow,
        m_position.x,
        m_position.y);

    s_windowMap[m_glfwWindow] = this;
}

void JzWindowSystem::SetupCallbacks()
{
    if (!m_glfwWindow) return;

    // Key callback
    glfwSetKeyCallback(m_glfwWindow, [](GLFWwindow *window, I32 key, I32 scancode, I32 action, I32 mods) {
        JzWindowSystem *instance = FindInstance(window);
        if (instance) {
            if (action == GLFW_PRESS)
                instance->KeyboardButtonPressedEvent.Broadcast(key);
            if (action == GLFW_RELEASE)
                instance->KeyboardButtonReleasedEvent.Broadcast(key);
        }
    });

    // Mouse button callback
    glfwSetMouseButtonCallback(m_glfwWindow, [](GLFWwindow *window, I32 button, I32 action, I32 mods) {
        JzWindowSystem *instance = FindInstance(window);
        if (instance) {
            if (action == GLFW_PRESS)
                instance->MouseButtonPressedEvent.Broadcast(button);
            if (action == GLFW_RELEASE)
                instance->MouseButtonReleasedEvent.Broadcast(button);
        }
    });

    // Scroll callback
    glfwSetScrollCallback(m_glfwWindow, [](GLFWwindow *window, F64 xOffset, F64 yOffset) {
        JzWindowSystem *instance = FindInstance(window);
        if (instance) {
            instance->MouseScrolledEvent.Broadcast({static_cast<F32>(xOffset), static_cast<F32>(yOffset)});
        }
    });

    // Window size callback
    glfwSetWindowSizeCallback(m_glfwWindow, [](GLFWwindow *window, I32 width, I32 height) {
        JzWindowSystem *instance = FindInstance(window);
        if (instance) {
            instance->WindowResizedEvent.Broadcast({width, height});
        }
    });

    // Framebuffer size callback
    glfwSetFramebufferSizeCallback(m_glfwWindow, [](GLFWwindow *window, I32 width, I32 height) {
        JzWindowSystem *instance = FindInstance(window);
        if (instance) {
            instance->WindowFrameBufferResizedEvent.Broadcast({width, height});
        }
    });

    // Cursor position callback
    glfwSetCursorPosCallback(m_glfwWindow, [](GLFWwindow *window, F64 x, F64 y) {
        JzWindowSystem *instance = FindInstance(window);
        if (instance) {
            instance->WindowCursorMoveEvent.Broadcast({static_cast<I32>(x), static_cast<I32>(y)});
        }
    });

    // Window position callback
    glfwSetWindowPosCallback(m_glfwWindow, [](GLFWwindow *window, I32 x, I32 y) {
        JzWindowSystem *instance = FindInstance(window);
        if (instance) {
            instance->WindowMoveEvent.Broadcast({x, y});
        }
    });

    // Iconify callback
    glfwSetWindowIconifyCallback(m_glfwWindow, [](GLFWwindow *window, I32 iconified) {
        JzWindowSystem *instance = FindInstance(window);
        if (instance) {
            if (iconified == GLFW_TRUE)
                instance->WindowMinimizedEvent.Broadcast();
            if (iconified == GLFW_FALSE)
                instance->WindowMaximizedEvent.Broadcast();
        }
    });

    // Focus callback
    glfwSetWindowFocusCallback(m_glfwWindow, [](GLFWwindow *window, I32 focused) {
        JzWindowSystem *instance = FindInstance(window);
        if (instance) {
            if (focused == GLFW_TRUE)
                instance->WindowFocusGainEvent.Broadcast();
            if (focused == GLFW_FALSE)
                instance->WindowFocusLostEvent.Broadcast();
        }
    });

    // Close callback
    glfwSetWindowCloseCallback(m_glfwWindow, [](GLFWwindow *window) {
        JzWindowSystem *instance = FindInstance(window);
        if (instance) {
            instance->WindowClosedEvent.Broadcast();
        }
    });
}

void JzWindowSystem::UpdateSizeLimit() const
{
    if (m_glfwWindow) {
        glfwSetWindowSizeLimits(
            m_glfwWindow,
            m_minimumSize.x,
            m_minimumSize.y,
            m_maximumSize.x,
            m_maximumSize.y);
    }
}

void JzWindowSystem::OnResize(JzIVec2 size)
{
    m_size = size;
}

void JzWindowSystem::OnMove(JzIVec2 position)
{
    if (!m_fullscreen) {
        m_position = position;
    }
}

JzWindowSystem *JzWindowSystem::FindInstance(GLFWwindow *glfwWindow)
{
    auto it = s_windowMap.find(glfwWindow);
    return it != s_windowMap.end() ? it->second : nullptr;
}

// ==================== ECS Update Helpers ====================

void JzWindowSystem::PollEvents(JzWorld &world)
{
    if (m_glfwWindow) {
        glfwPollEvents();
    }
}

void JzWindowSystem::UpdateWindowState(JzWorld &world)
{
    if (!m_glfwWindow) return;

    auto view = world.View<JzWindowStateComponent>();
    for (auto entity : view) {
        auto &state = world.GetComponent<JzWindowStateComponent>(entity);

        // Update from backend
        state.size            = GetSize();
        state.position        = GetPosition();
        state.framebufferSize = GetFramebufferSize();
        state.title           = GetTitle();
        state.focused         = IsFocused();
        state.visible         = IsVisible();
        state.shouldClose     = ShouldClose();

        // Update state enum
        if (IsMinimized()) {
            state.state = JzEWindowState::Minimized;
        } else if (IsMaximized()) {
            state.state = JzEWindowState::Maximized;
        } else if (IsFullscreen()) {
            state.state = JzEWindowState::Fullscreen;
        } else if (IsHidden()) {
            state.state = JzEWindowState::Hidden;
        } else {
            state.state = JzEWindowState::Normal;
        }

        // Store native handle
        state.nativeHandle = GetNativeWindow();
    }
}

void JzWindowSystem::UpdateInputState(JzWorld &world)
{
    if (!m_glfwWindow) return;

    auto view = world.View<JzWindowStateComponent, JzInputStateComponent>();
    for (auto entity : view) {
        SyncInputFromBackend(world, entity);
    }
}

void JzWindowSystem::SyncInputFromBackend(JzWorld &world, JzEntity windowEntity)
{
    if (!m_glfwWindow) return;

    auto &input = world.GetComponent<JzInputStateComponent>(windowEntity);

    // Get current mouse position
    F64 mouseX, mouseY;
    glfwGetCursorPos(m_glfwWindow, &mouseX, &mouseY);
    JzVec2 currentMousePos(static_cast<F32>(mouseX), static_cast<F32>(mouseY));

    // Calculate mouse delta
    if (!input.firstFrame) {
        input.mouse.positionDelta = currentMousePos - input.mouse.lastPosition;
    } else {
        input.mouse.positionDelta = JzVec2(0.0f, 0.0f);
    }

    input.mouse.lastPosition = input.mouse.position;
    input.mouse.position     = currentMousePos;
    input.firstFrame         = false;

    // Update keyboard state
    auto updateKey = [&](I32 key) {
        Bool pressed    = glfwGetKey(m_glfwWindow, key) == GLFW_PRESS;
        Bool wasPressed = input.keyboard.keysPressed[static_cast<Size>(key)];

        if (pressed && !wasPressed) {
            input.keyboard.keysDown.set(static_cast<Size>(key));
        }
        if (!pressed && wasPressed) {
            input.keyboard.keysUp.set(static_cast<Size>(key));
        }

        if (pressed) {
            input.keyboard.keysPressed.set(static_cast<Size>(key));
        } else {
            input.keyboard.keysPressed.reset(static_cast<Size>(key));
        }
    };

    // Update common keys
    for (I32 key = GLFW_KEY_SPACE; key <= GLFW_KEY_LAST; ++key) {
        if (key < static_cast<I32>(JzInputStateComponent::KeyboardState::KEY_COUNT)) {
            updateKey(key);
        }
    }

    // Update mouse buttons
    for (I32 button = 0; button < static_cast<I32>(JzInputStateComponent::MouseState::BUTTON_COUNT); ++button) {
        Bool pressed    = glfwGetMouseButton(m_glfwWindow, button) == GLFW_PRESS;
        Bool wasPressed = input.mouse.buttonsPressed[static_cast<Size>(button)];

        if (pressed && !wasPressed) {
            input.mouse.buttonsDown.set(static_cast<Size>(button));
        }
        if (!pressed && wasPressed) {
            input.mouse.buttonsUp.set(static_cast<Size>(button));
        }

        if (pressed) {
            input.mouse.buttonsPressed.set(static_cast<Size>(button));
        } else {
            input.mouse.buttonsPressed.reset(static_cast<Size>(button));
        }
    }
}

void JzWindowSystem::ProcessWindowEvents(JzWorld &world)
{
    // Get event dispatcher (optional - gracefully skip if not registered)
    JzEventDispatcherSystem *dispatcher = nullptr;
    try {
        dispatcher = &JzServiceContainer::Get<JzEventDispatcherSystem>();
    } catch (...) {
        dispatcher = nullptr;
    }

    auto view = world.View<JzWindowEventQueueComponent>();
    for (auto entity : view) {
        auto &queue = world.GetComponent<JzWindowEventQueueComponent>(entity);

        for (const auto &event : queue.events) {
            switch (event.type) {
                case JzEWindowEventType::Resized:
                    break;
                case JzEWindowEventType::FocusGained:
                case JzEWindowEventType::FocusLost:
                    break;
                case JzEWindowEventType::Closed:
                    if (world.HasComponent<JzWindowStateComponent>(entity)) {
                        world.GetComponent<JzWindowStateComponent>(entity).shouldClose = true;
                    }
                    break;
                case JzEWindowEventType::FileDropped:
                    if (dispatcher) {
                        JzFileDroppedEvent ecsEvent;
                        ecsEvent.source       = entity;
                        ecsEvent.filePaths    = event.droppedPaths;
                        ecsEvent.dropPosition = event.dropPosition;
                        dispatcher->Send(std::move(ecsEvent));
                    }
                    break;
                case JzEWindowEventType::FramebufferResized:
                    if (dispatcher) {
                        JzWindowFramebufferResizedEvent ecsEvent;
                        ecsEvent.source = entity;
                        ecsEvent.size   = JzIVec2(event.data.resized.width, event.data.resized.height);
                        dispatcher->Send(std::move(ecsEvent));
                    }
                    break;
                case JzEWindowEventType::ContentScaleChanged:
                    if (dispatcher) {
                        JzWindowContentScaleChangedEvent ecsEvent;
                        ecsEvent.source = entity;
                        ecsEvent.scale  = JzVec2(event.data.contentScale.xScale, event.data.contentScale.yScale);
                        dispatcher->Send(std::move(ecsEvent));
                    }
                    break;
                default:
                    break;
            }
        }

        queue.Clear();
    }
}

void JzWindowSystem::ApplyComponentChanges(JzWorld &world)
{
    if (!m_glfwWindow) return;

    auto view = world.View<JzWindowStateComponent>();
    for (auto entity : view) {
        auto &state = world.GetComponent<JzWindowStateComponent>(entity);

        // Apply dirty size changes
        if (state.sizeDirty) {
            SetSize(state.size);
            state.sizeDirty = false;
        }

        // Apply dirty state changes
        if (state.stateDirty) {
            switch (state.state) {
                case JzEWindowState::Fullscreen:
                    SetFullscreen(true);
                    break;
                case JzEWindowState::Normal:
                    if (IsFullscreen()) {
                        SetFullscreen(false);
                    }
                    if (IsMinimized()) {
                        Restore();
                    }
                    break;
                case JzEWindowState::Minimized:
                    Minimize();
                    break;
                case JzEWindowState::Maximized:
                    Maximize();
                    break;
                case JzEWindowState::Hidden:
                    Hide();
                    break;
            }
            state.stateDirty = false;
        }
    }
}

void JzWindowSystem::HandleCloseRequests(JzWorld &world)
{
    auto view = world.View<JzWindowStateComponent>();
    for (auto entity : view) {
        auto &state = world.GetComponent<JzWindowStateComponent>(entity);

        if (state.shouldClose) {
            if (world.HasComponent<JzPersistentWindowTag>(entity)) {
                state.shouldClose = false;
                SetShouldClose(false);
            }
        }
    }
}

void JzWindowSystem::SwapBuffers(JzWorld &world)
{
    // Typically called by JzRERuntime after all rendering
}

void JzWindowSystem::UpdateStatistics(JzWorld &world, F32 delta)
{
    m_accumulatedTime += static_cast<F64>(delta);
    m_frameCount++;

    if (m_accumulatedTime >= 1.0) {
        F64 fps = static_cast<F64>(m_frameCount) / m_accumulatedTime;

        auto view = world.View<JzWindowStateComponent>();
        for (auto entity : view) {
            auto &state            = world.GetComponent<JzWindowStateComponent>(entity);
            state.stats.averageFPS = fps;
            state.stats.frameTime  = 1000.0 / fps;
            state.stats.frameCount++;
        }

        m_accumulatedTime = 0.0;
        m_frameCount      = 0;
    }
}

void JzWindowSystem::EmitWindowEvents(JzWorld &world)
{
    // Get event dispatcher (optional - gracefully skip if not registered)
    JzEventDispatcherSystem *dispatcher = nullptr;
    try {
        dispatcher = &JzServiceContainer::Get<JzEventDispatcherSystem>();
    } catch (...) {
        return;
    }

    auto view = world.View<JzWindowStateComponent>();
    for (auto entity : view) {
        const auto &state = world.GetComponent<JzWindowStateComponent>(entity);

        // On first frame, initialize cached state without emitting events
        if (!m_eventStateInitialized) {
            m_prevSize              = state.size;
            m_prevPosition          = state.position;
            m_prevFocused           = state.focused;
            m_prevMinimized         = state.IsMinimized();
            m_prevMaximized         = state.IsMaximized();
            m_prevShouldClose       = state.shouldClose;
            m_eventStateInitialized = true;
            continue;
        }

        // Window resized
        if (state.size != m_prevSize) {
            JzWindowResizedEvent ecsEvent;
            ecsEvent.source  = entity;
            ecsEvent.size    = state.size;
            ecsEvent.oldSize = m_prevSize;
            dispatcher->Send(std::move(ecsEvent));
        }

        // Window moved
        if (state.position != m_prevPosition) {
            JzWindowMovedEvent ecsEvent;
            ecsEvent.source   = entity;
            ecsEvent.position = state.position;
            dispatcher->Send(std::move(ecsEvent));
        }

        // Focus changed
        if (state.focused != m_prevFocused) {
            JzWindowFocusEvent ecsEvent;
            ecsEvent.source  = entity;
            ecsEvent.focused = state.focused;
            dispatcher->Send(std::move(ecsEvent));
        }

        // Minimized / restored
        Bool currentMinimized = state.IsMinimized();
        if (currentMinimized != m_prevMinimized) {
            JzWindowIconifiedEvent ecsEvent;
            ecsEvent.source    = entity;
            ecsEvent.iconified = currentMinimized;
            dispatcher->Send(std::move(ecsEvent));
        }

        // Maximized
        Bool currentMaximized = state.IsMaximized();
        if (currentMaximized != m_prevMaximized) {
            JzWindowMaximizedEvent ecsEvent;
            ecsEvent.source    = entity;
            ecsEvent.maximized = currentMaximized;
            dispatcher->Send(std::move(ecsEvent));
        }

        // Close request
        if (state.shouldClose && !m_prevShouldClose) {
            JzWindowClosedEvent ecsEvent;
            ecsEvent.source = entity;
            dispatcher->Send(std::move(ecsEvent));
        }

        // Update cached state
        m_prevSize        = state.size;
        m_prevPosition    = state.position;
        m_prevFocused     = state.focused;
        m_prevMinimized   = currentMinimized;
        m_prevMaximized   = currentMaximized;
        m_prevShouldClose = state.shouldClose;
    }
}

} // namespace JzRE
