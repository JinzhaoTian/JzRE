/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Core/JzDelegate.h"
#include "JzRE/Runtime/Core/JzVector.h"
#include "JzRE/Runtime/Platform/RHI/JzDevice.h"
#include "JzRE/Runtime/Platform/Window/JzWindowConfig.h"

namespace JzRE {

/**
 * @brief Abstract window backend interface.
 *
 * Abstracts the windowing library (GLFW, SDL, etc.) from the engine.
 * JzWindowSystem delegates all platform-specific window operations to this interface.
 *
 * Implementations:
 * - JzGLFWWindowBackend (default)
 */
class IWindowBackend {
public:
    virtual ~IWindowBackend() = default;

    // ==================== Lifecycle ====================

    /**
     * @brief Initialize the backend and create the window.
     *
     * @param rhiType The RHI type for setting context hints.
     * @param config Window configuration.
     */
    virtual void Initialize(JzERHIType rhiType, const JzWindowConfig &config) = 0;

    /**
     * @brief Destroy the window and shut down the backend.
     */
    virtual void Shutdown() = 0;

    /**
     * @brief Check if the backend has a valid window.
     */
    virtual Bool IsValid() const = 0;

    // ==================== Context Management ====================

    virtual void MakeContextCurrent() = 0;
    virtual void DetachContext()      = 0;
    virtual void SwapBuffers()        = 0;

    // ==================== Event Polling ====================

    virtual void PollEvents() = 0;

    // ==================== Window Properties ====================

    virtual void   SetTitle(const String &title) = 0;
    virtual String GetTitle() const              = 0;

    virtual void    SetPosition(JzIVec2 position) = 0;
    virtual JzIVec2 GetPosition() const           = 0;

    virtual void    SetSize(JzIVec2 size) = 0;
    virtual JzIVec2 GetSize() const       = 0;

    virtual JzIVec2 GetFramebufferSize() const = 0;
    virtual JzIVec2 GetMonitorSize() const     = 0;

    virtual void    SetMinimumSize(JzIVec2 size) = 0;
    virtual JzIVec2 GetMinimumSize() const       = 0;

    virtual void    SetMaximumSize(JzIVec2 size) = 0;
    virtual JzIVec2 GetMaximumSize() const       = 0;

    // ==================== Window State ====================

    virtual void SetFullscreen(Bool value) = 0;
    virtual Bool IsFullscreen() const      = 0;

    virtual void SetAlignCentered() = 0;

    virtual Bool IsMinimized() const = 0;
    virtual void Minimize()          = 0;
    virtual void Restore()           = 0;

    virtual Bool IsMaximized() const = 0;
    virtual void Maximize()          = 0;

    virtual Bool IsHidden() const  = 0;
    virtual Bool IsVisible() const = 0;
    virtual void Hide()            = 0;
    virtual void Show()            = 0;

    virtual Bool IsFocused() const = 0;
    virtual void Focus()           = 0;

    virtual void SetShouldClose(Bool value) = 0;
    virtual Bool ShouldClose() const        = 0;

    // ==================== Input Polling ====================

    /**
     * @brief Poll the current state of a keyboard key.
     * @return true if pressed, false if released.
     */
    virtual Bool GetKeyState(I32 key) const = 0;

    /**
     * @brief Poll the current state of a mouse button.
     * @return true if pressed, false if released.
     */
    virtual Bool GetMouseButtonState(I32 button) const = 0;

    /**
     * @brief Get the current cursor position in window coordinates.
     */
    virtual JzVec2 GetCursorPosition() const = 0;

    // ==================== Platform Handles ====================

    /**
     * @brief Get the windowing library's window handle (e.g., GLFWwindow*).
     *
     * Used for library-specific integrations such as ImGui backends.
     */
    virtual void *GetPlatformWindowHandle() const = 0;

    /**
     * @brief Get the OS-native window handle.
     *
     * Returns HWND on Windows, NSWindow* on macOS, X11 Window on Linux.
     */
    virtual void *GetNativeWindowHandle() const = 0;

    // ==================== Event Delegates ====================
    // The backend fires these when window/input events occur from callbacks.
    // JzWindowSystem subscribes to forward them into ECS.

    JzDelegate<I32>     OnKeyPressed;
    JzDelegate<I32>     OnKeyReleased;
    JzDelegate<I32>     OnMouseButtonPressed;
    JzDelegate<I32>     OnMouseButtonReleased;
    JzDelegate<JzIVec2> OnMouseMoved;
    JzDelegate<JzVec2>  OnMouseScrolled;

    JzDelegate<JzIVec2> OnWindowResized;
    JzDelegate<JzIVec2> OnFrameBufferResized;
    JzDelegate<JzIVec2> OnWindowMoved;
    JzDelegate<JzIVec2> OnCursorMoved;
    JzDelegate<>        OnWindowMinimized;
    JzDelegate<>        OnWindowMaximized;
    JzDelegate<>        OnWindowFocusGained;
    JzDelegate<>        OnWindowFocusLost;
    JzDelegate<>        OnWindowClosed;
};

} // namespace JzRE
