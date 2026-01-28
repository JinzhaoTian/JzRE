/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Function/ECS/JzWindowSystem.h"

#include "JzRE/Runtime/Core/JzServiceContainer.h"
#include "JzRE/Runtime/Function/Event/JzEventDispatcherSystem.h"
#include "JzRE/Runtime/Function/Event/JzWindowEvents.h"
#include "JzRE/Runtime/Platform/Window/IWindowBackend.h"
#include "JzRE/Runtime/Platform/Window/JzGLFWWindowBackend.h"

namespace JzRE {

// ==================== Lifecycle ====================

JzWindowSystem::JzWindowSystem() = default;

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
    if (!m_backend || !m_backend->IsValid()) return;

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

// ==================== Backend Management ====================

void JzWindowSystem::SetBackend(std::unique_ptr<IWindowBackend> backend)
{
    m_backend = std::move(backend);
}

// ==================== Window Creation ====================

void JzWindowSystem::InitializeWindow(JzERHIType rhiType, const JzWindowConfig &config)
{
    // Create default GLFW backend if none was set
    if (!m_backend) {
        m_backend = std::make_unique<JzGLFWWindowBackend>();
    }

    m_backend->Initialize(rhiType, config);
    WireBackendDelegates();
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
    if (m_backend) {
        m_backend->Shutdown();
        m_backend.reset();
    }
}

// ==================== Window Operations ====================

void JzWindowSystem::MakeCurrentContext() const
{
    if (m_backend) {
        m_backend->MakeContextCurrent();
    }
}

void JzWindowSystem::DetachContext() const
{
    if (m_backend) {
        m_backend->DetachContext();
    }
}

void JzWindowSystem::PollWindowEvents() const
{
    if (m_backend) {
        m_backend->PollEvents();
    }
}

void JzWindowSystem::SwapWindowBuffers() const
{
    if (m_backend) {
        m_backend->SwapBuffers();
    }
}

// ==================== Window Properties ====================

void *JzWindowSystem::GetPlatformWindowHandle() const
{
    return m_backend ? m_backend->GetPlatformWindowHandle() : nullptr;
}

void *JzWindowSystem::GetNativeWindow() const
{
    return m_backend ? m_backend->GetNativeWindowHandle() : nullptr;
}

void JzWindowSystem::SetTitle(const String &title)
{
    if (m_backend) {
        m_backend->SetTitle(title);
    }
}

String JzWindowSystem::GetTitle() const
{
    return m_backend ? m_backend->GetTitle() : String{};
}

void JzWindowSystem::SetPosition(JzIVec2 position)
{
    if (m_backend) {
        m_backend->SetPosition(position);
    }
}

JzIVec2 JzWindowSystem::GetPosition() const
{
    return m_backend ? m_backend->GetPosition() : JzIVec2{0, 0};
}

void JzWindowSystem::SetSize(JzIVec2 size)
{
    if (m_backend) {
        m_backend->SetSize(size);
    }
}

JzIVec2 JzWindowSystem::GetSize() const
{
    return m_backend ? m_backend->GetSize() : JzIVec2{0, 0};
}

JzIVec2 JzWindowSystem::GetFramebufferSize() const
{
    return m_backend ? m_backend->GetFramebufferSize() : JzIVec2{0, 0};
}

JzIVec2 JzWindowSystem::GetMonitorSize() const
{
    return m_backend ? m_backend->GetMonitorSize() : JzIVec2{0, 0};
}

void JzWindowSystem::SetMinimumSize(JzIVec2 minimumSize)
{
    if (m_backend) {
        m_backend->SetMinimumSize(minimumSize);
    }
}

JzIVec2 JzWindowSystem::GetMinimumSize() const
{
    return m_backend ? m_backend->GetMinimumSize() : JzIVec2{-1, -1};
}

void JzWindowSystem::SetMaximumSize(JzIVec2 maximumSize)
{
    if (m_backend) {
        m_backend->SetMaximumSize(maximumSize);
    }
}

JzIVec2 JzWindowSystem::GetMaximumSize() const
{
    return m_backend ? m_backend->GetMaximumSize() : JzIVec2{-1, -1};
}

// ==================== Window State ====================

void JzWindowSystem::SetFullscreen(Bool value)
{
    if (m_backend) {
        m_backend->SetFullscreen(value);
    }
}

Bool JzWindowSystem::IsFullscreen() const
{
    return m_backend ? m_backend->IsFullscreen() : false;
}

void JzWindowSystem::SetAlignCentered()
{
    if (m_backend) {
        m_backend->SetAlignCentered();
    }
}

Bool JzWindowSystem::IsMinimized() const
{
    return m_backend ? m_backend->IsMinimized() : false;
}

void JzWindowSystem::Minimize() const
{
    if (m_backend) {
        m_backend->Minimize();
    }
}

void JzWindowSystem::Restore() const
{
    if (m_backend) {
        m_backend->Restore();
    }
}

Bool JzWindowSystem::IsMaximized() const
{
    return m_backend ? m_backend->IsMaximized() : false;
}

void JzWindowSystem::Maximize() const
{
    if (m_backend) {
        m_backend->Maximize();
    }
}

Bool JzWindowSystem::IsHidden() const
{
    return m_backend ? m_backend->IsHidden() : false;
}

Bool JzWindowSystem::IsVisible() const
{
    return m_backend ? m_backend->IsVisible() : false;
}

void JzWindowSystem::Hide() const
{
    if (m_backend) {
        m_backend->Hide();
    }
}

void JzWindowSystem::Show() const
{
    if (m_backend) {
        m_backend->Show();
    }
}

Bool JzWindowSystem::IsFocused() const
{
    return m_backend ? m_backend->IsFocused() : false;
}

void JzWindowSystem::Focus() const
{
    if (m_backend) {
        m_backend->Focus();
    }
}

void JzWindowSystem::SetShouldClose(Bool value) const
{
    if (m_backend) {
        m_backend->SetShouldClose(value);
    }
}

Bool JzWindowSystem::ShouldClose() const
{
    return m_backend ? m_backend->ShouldClose() : true;
}

// ==================== Input Polling ====================

Bool JzWindowSystem::GetKeyState(I32 key) const
{
    return m_backend ? m_backend->GetKeyState(key) : false;
}

Bool JzWindowSystem::GetMouseButtonState(I32 button) const
{
    return m_backend ? m_backend->GetMouseButtonState(button) : false;
}

JzVec2 JzWindowSystem::GetCursorPosition() const
{
    return m_backend ? m_backend->GetCursorPosition() : JzVec2{0.0f, 0.0f};
}

// ==================== Backend Delegate Wiring ====================

void JzWindowSystem::WireBackendDelegates()
{
    if (!m_backend) return;

    // Accumulate scroll delta from backend callbacks.
    // Scroll has no pollable state, so we must capture it from the callback
    // and apply it during SyncInputFromBackend().
    m_backend->OnMouseScrolled += [this](JzVec2 scroll) {
        m_pendingScrollDelta += scroll;
    };

    // All other backend events are handled through:
    // - Window events: EmitWindowEvents() uses change detection on JzWindowStateComponent
    // - Input state: SyncInputFromBackend() polls keyboard/mouse state from the backend
    // No delegate forwarding is needed; events flow through JzEventDispatcherSystem.
}

// ==================== ECS Update Helpers ====================

void JzWindowSystem::PollEvents(JzWorld &world)
{
    if (m_backend) {
        m_backend->PollEvents();
    }
}

void JzWindowSystem::UpdateWindowState(JzWorld &world)
{
    if (!m_backend || !m_backend->IsValid()) return;

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
    if (!m_backend || !m_backend->IsValid()) return;

    auto view = world.View<JzWindowStateComponent, JzInputStateComponent>();
    for (auto entity : view) {
        SyncInputFromBackend(world, entity);
    }
}

void JzWindowSystem::SyncInputFromBackend(JzWorld &world, JzEntity windowEntity)
{
    if (!m_backend || !m_backend->IsValid()) return;

    auto &input = world.GetComponent<JzInputStateComponent>(windowEntity);

    // Get current mouse position
    JzVec2 currentMousePos = m_backend->GetCursorPosition();

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
        Bool pressed    = m_backend->GetKeyState(key);
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

    // Update common keys (GLFW key range: KEY_SPACE=32 to KEY_LAST=348)
    for (I32 key = 32; key <= 348; ++key) {
        if (key < static_cast<I32>(JzInputStateComponent::KeyboardState::KEY_COUNT)) {
            updateKey(key);
        }
    }

    // Update mouse buttons
    for (I32 button = 0; button < static_cast<I32>(JzInputStateComponent::MouseState::BUTTON_COUNT); ++button) {
        Bool pressed    = m_backend->GetMouseButtonState(button);
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

    // Apply accumulated scroll delta from backend callbacks
    input.mouse.scrollDelta = m_pendingScrollDelta;
    m_pendingScrollDelta    = JzVec2(0.0f, 0.0f);
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
    if (!m_backend || !m_backend->IsValid()) return;

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
