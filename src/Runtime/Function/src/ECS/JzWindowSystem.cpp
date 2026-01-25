/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Function/ECS/JzWindowSystem.h"

#include "JzRE/Runtime/Core/JzServiceContainer.h"
#include "JzRE/Runtime/Function/Window/JzWindow.h"

namespace JzRE {

void JzWindowSystem::OnInit(JzWorld &world)
{
    // Window entity should be created externally (by JzRERuntime) and registered
    // This system just manages the synchronization
}

void JzWindowSystem::Update(JzWorld &world, F32 delta)
{
    // Poll events from backend
    PollEvents(world);

    // Update window state components from backend
    UpdateWindowState(world);

    // Update input state from window callbacks
    UpdateInputState(world);

    // Process window events
    ProcessWindowEvents(world);

    // Apply any component changes to backend
    ApplyComponentChanges(world);

    // Handle close requests
    HandleCloseRequests(world);

    // Update statistics
    UpdateStatistics(world, delta);
}

void JzWindowSystem::OnShutdown(JzWorld &world)
{
    // Cleanup is handled by JzRERuntime which owns the JzWindow instance
}

JzEntity JzWindowSystem::CreateWindowEntity(JzWorld &world, const JzWindowConfig &config)
{
    JzEntity entity = world.CreateEntity();

    // Add window state component
    auto &windowState         = world.AddComponent<JzWindowStateComponent>(entity);
    windowState.title         = config.title;
    windowState.size          = JzIVec2(config.width, config.height);
    windowState.framebufferSize = windowState.size;
    windowState.resizable     = config.resizable;
    windowState.decorated     = config.decorated;
    windowState.floating      = config.floating;
    windowState.visible       = config.visible;
    windowState.swapInterval  = config.vsync ? 1 : 0;

    if (config.fullscreen) {
        windowState.state = JzEWindowState::Fullscreen;
    }

    // Add event queue component
    world.AddComponent<JzWindowEventQueueComponent>(entity);

    // Add input state component for this window
    world.AddComponent<JzInputStateComponent>(entity);

    return entity;
}

void JzWindowSystem::PollEvents(JzWorld &world)
{
    // Get window backend from service container
    JzWindow *windowPtr = nullptr;
    try {
        windowPtr = &JzServiceContainer::Get<JzWindow>();
    } catch (...) {
        return;
    }

    // Poll GLFW events
    windowPtr->PollEvents();
}

void JzWindowSystem::UpdateWindowState(JzWorld &world)
{
    // Get window backend
    JzWindow *windowPtr = nullptr;
    try {
        windowPtr = &JzServiceContainer::Get<JzWindow>();
    } catch (...) {
        return;
    }

    auto &window = *windowPtr;

    // Update all window state components
    auto view = world.View<JzWindowStateComponent>();
    for (auto entity : view) {
        auto &state = world.GetComponent<JzWindowStateComponent>(entity);

        // Update from backend
        state.size            = window.GetSize();
        state.position        = window.GetPosition();
        state.framebufferSize = window.GetFramebufferSize();
        state.title           = window.GetTitle();
        state.focused         = window.IsFocused();
        state.visible         = window.IsVisible();
        state.shouldClose     = window.ShouldClose();

        // Update state enum
        if (window.IsMinimized()) {
            state.state = JzEWindowState::Minimized;
        } else if (window.IsMaximized()) {
            state.state = JzEWindowState::Maximized;
        } else if (window.IsFullscreen()) {
            state.state = JzEWindowState::Fullscreen;
        } else if (window.IsHidden()) {
            state.state = JzEWindowState::Hidden;
        } else {
            state.state = JzEWindowState::Normal;
        }

        // Store native handle
        state.nativeHandle = window.GetNativeWindow();
    }
}

void JzWindowSystem::UpdateInputState(JzWorld &world)
{
    // Get window backend
    JzWindow *windowPtr = nullptr;
    try {
        windowPtr = &JzServiceContainer::Get<JzWindow>();
    } catch (...) {
        return;
    }

    // Update input state for each window entity
    auto view = world.View<JzWindowStateComponent, JzInputStateComponent>();
    for (auto entity : view) {
        SyncInputFromBackend(world, entity);
    }
}

void JzWindowSystem::SyncInputFromBackend(JzWorld &world, JzEntity windowEntity)
{
    JzWindow *windowPtr = nullptr;
    try {
        windowPtr = &JzServiceContainer::Get<JzWindow>();
    } catch (...) {
        return;
    }

    auto &window = *windowPtr;
    auto &input  = world.GetComponent<JzInputStateComponent>(windowEntity);

    // Get current mouse position
    F64 mouseX, mouseY;
    glfwGetCursorPos(window.GetGLFWWindow(), &mouseX, &mouseY);
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
        Bool pressed = glfwGetKey(window.GetGLFWWindow(), key) == GLFW_PRESS;
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
        Bool pressed = glfwGetMouseButton(window.GetGLFWWindow(), button) == GLFW_PRESS;
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

    // Note: Scroll is handled via callback and accumulated in JzWindow
    // We'll sync it via the input manager for now
}

void JzWindowSystem::ProcessWindowEvents(JzWorld &world)
{
    auto view = world.View<JzWindowEventQueueComponent>();
    for (auto entity : view) {
        auto &queue = world.GetComponent<JzWindowEventQueueComponent>(entity);

        for (const auto &event : queue.events) {
            switch (event.type) {
                case JzEWindowEventType::Resized:
                    // Handle resize event if needed
                    break;
                case JzEWindowEventType::FocusGained:
                case JzEWindowEventType::FocusLost:
                    // Handle focus change if needed
                    break;
                case JzEWindowEventType::Closed:
                    // Handle close request
                    if (world.HasComponent<JzWindowStateComponent>(entity)) {
                        world.GetComponent<JzWindowStateComponent>(entity).shouldClose = true;
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
    JzWindow *windowPtr = nullptr;
    try {
        windowPtr = &JzServiceContainer::Get<JzWindow>();
    } catch (...) {
        return;
    }

    auto &window = *windowPtr;

    auto view = world.View<JzWindowStateComponent>();
    for (auto entity : view) {
        auto &state = world.GetComponent<JzWindowStateComponent>(entity);

        // Apply dirty size changes
        if (state.sizeDirty) {
            window.SetSize(state.size);
            state.sizeDirty = false;
        }

        // Apply dirty state changes
        if (state.stateDirty) {
            switch (state.state) {
                case JzEWindowState::Fullscreen:
                    window.SetFullscreen(true);
                    break;
                case JzEWindowState::Normal:
                    if (window.IsFullscreen()) {
                        window.SetFullscreen(false);
                    }
                    if (window.IsMinimized()) {
                        window.Restore();
                    }
                    break;
                case JzEWindowState::Minimized:
                    window.Minimize();
                    break;
                case JzEWindowState::Maximized:
                    window.Maximize();
                    break;
                case JzEWindowState::Hidden:
                    window.Hide();
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
            // Check if window should persist
            if (world.HasComponent<JzPersistentWindowTag>(entity)) {
                state.shouldClose = false;
                // Clear the close flag on the backend
                try {
                    auto &window = JzServiceContainer::Get<JzWindow>();
                    window.SetShouldClose(false);
                } catch (...) { }
            }
        }
    }
}

void JzWindowSystem::SwapBuffers(JzWorld &world)
{
    // This is typically called by JzRERuntime after all rendering
    // Not needed in the window system itself
}

void JzWindowSystem::UpdateStatistics(JzWorld &world, F32 delta)
{
    m_accumulatedTime += static_cast<F64>(delta);
    m_frameCount++;

    if (m_accumulatedTime >= 1.0) {
        F64 fps = static_cast<F64>(m_frameCount) / m_accumulatedTime;

        auto view = world.View<JzWindowStateComponent>();
        for (auto entity : view) {
            auto &state = world.GetComponent<JzWindowStateComponent>(entity);
            state.stats.averageFPS = fps;
            state.stats.frameTime  = 1000.0 / fps;
            state.stats.frameCount++;
        }

        m_accumulatedTime = 0.0;
        m_frameCount      = 0;
    }
}

} // namespace JzRE
