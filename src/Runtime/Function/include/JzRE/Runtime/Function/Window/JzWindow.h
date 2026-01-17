/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <unordered_map>
#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif
#include <GLFW/glfw3.h>
#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Core/JzEvent.h"
#include "JzRE/Runtime/Core/JzVector.h"
#include "JzRE/Runtime/Platform/JzDevice.h"
#include "JzRE/Runtime/Function/Window/JzWindowSettings.h"

namespace JzRE {

/**
 * @brief Window of JzRE
 */
class JzWindow {
public:
    /**
     * @brief Constructor
     */
    JzWindow(JzERHIType rhiType, const JzWindowSettings &windowSettings);

    /**
     * @brief Destructor
     */
    ~JzWindow();

    /**
     * @brief Make the current context
     */
    void MakeCurrentContext() const;

    /**
     * @brief Detach the OpenGL context from the current thread
     *
     * This is required for multi-threaded rendering. After detaching,
     * another thread can call MakeCurrentContext() to acquire the context.
     */
    void DetachContext() const;

    /**
     * @brief Get the GLFW window
     *
     * @return GLFWwindow*
     */
    GLFWwindow *GetGLFWWindow() const;

    /**
     * @brief Get Native window Handle
     *
     * @return void* platform specific
     */
    void *GetNativeWindow() const;

    /**
     * @brief Get the size, in pixels, of the primary monitor
     */
    JzIVec2 GetMonitorSize() const;

    /**
     * @brief Set the title of the window
     *
     * @param title
     */
    void SetTitle(const String &title);

    /**
     * @brief Get the title of the window
     */
    String GetTitle() const;

    /**
     * @brief Set the position for the window
     */
    void SetPosition(JzIVec2 p_position);

    /**
     * @brief Get the Position of the window
     *
     * @return JzIVec2
     */
    JzIVec2 GetPosition() const;

    /**
     * @brief Set the size for the window
     *
     * @param p_size
     */
    void SetSize(JzIVec2 p_size);

    /**
     * @brief Get the current size of the window
     *
     * @return JzIVec2
     */
    JzIVec2 GetSize() const;

    /**
     * @brief Get the framebuffer size in pixels
     *
     * On high-DPI displays (e.g., macOS Retina), the framebuffer size
     * may be larger than the window size. Use this for rendering operations.
     *
     * @return JzIVec2 Framebuffer size in pixels
     */
    JzIVec2 GetFramebufferSize() const;

    /**
     * @brief Is the windows minimized
     *
     * @return Bool
     */
    Bool IsMinimized() const;

    /**
     * @brief Minimize the window if it was previously visible
     */
    void Minimize() const;

    /**
     * @brief Restore the window if it was previously minimized
     */
    void Restore() const;

    /**
     * @brief Set a minimum size for the window
     *
     * @param minimumSize
     */
    void SetMinimumSize(JzIVec2 minimumSize);

    /**
     * @brief Get the current minimum size of the window
     *
     * @return JzIVec2
     */
    JzIVec2 GetMinimumSize() const;

    /**
     * @brief Is the windows maximized
     *
     * @return Bool
     */
    Bool IsMaximized() const;

    /**
     * @brief Maximize the window
     */
    void Maximize() const;

    /**
     * @brief Set a maximum size for the window
     *
     * @param maximumSize
     */
    void SetMaximumSize(JzIVec2 maximumSize);

    /**
     * @brief Get the current maximum size of the window
     *
     * @return JzIVec2
     */
    JzIVec2 GetMaximumSize() const;

    /**
     * @brief Set the window in fullscreen mode
     *
     * @param p_value
     */
    void SetFullscreen(Bool p_value);

    /**
     * @brief Is the window fullscreen
     *
     * @return Bool
     */
    Bool IsFullscreen() const;

    /**
     * @brief Set the window align centered
     */
    void SetAlignCentered();

    /**
     * @brief Is the window hidden
     *
     * @return bool
     */
    Bool IsHidden() const;

    /**
     * @brief Is the window visible
     *
     * @return Bool
     */
    Bool IsVisible() const;

    /**
     * @brief Hides the window if it was previously visible
     */
    void Hide() const;

    /**
     * @brief Show the window if it was previously hidden
     */
    void Show() const;

    /**
     * @brief Is the windows focused
     *
     * @return Bool
     */
    Bool IsFocused() const;

    /**
     * @brief Focus the window
     */
    void Focus() const;

    /**
     * @brief Poll events
     */
    void PollEvents() const;

    /**
     * @brief Swap the buffers
     */
    void SwapBuffers() const;

    /**
     * @brief Set the should close flag
     *
     * @param value
     */
    void SetShouldClose(Bool value) const;

    /**
     * @brief Check if the window should close
     *
     * @return Bool
     */
    Bool ShouldClose() const;

    /**
     * @brief Find the instance
     *
     * @param p_glfwWindow
     * @return JzWindow*
     */
    static JzWindow *FindInstance(GLFWwindow *p_glfwWindow);

private:
    /**
     * @brief Create the GLFW window
     *
     * @param windowSettings
     */
    void CreateGlfwWindow(const JzWindowSettings &windowSettings);

    void BindKeyCallback() const;               // Bind the key callback
    void BindMouseCallback() const;             // Bind the mouse callback
    void BindScrollCallback() const;            // Bind the scroll callback
    void BindIconifyCallback() const;           // Bind the iconify callback
    void BindCloseCallback() const;             // Bind the close callback
    void BindResizeCallback() const;            // Bind the resize callback
    void BindCursorMoveCallback() const;        // Bind the cursor move callback
    void BindFramebufferResizeCallback() const; // Bind the framebuffer resize callback
    void BindMoveCallback() const;              // Bind the move callback
    void BindFocusCallback() const;             // Bind the focus callback

    void OnResize(JzIVec2 p_size);
    void OnMove(JzIVec2 p_position);

    void UpdateSizeLimit() const;

public:
    /* Inputs events */
    JzEvent<I32>     KeyboardButtonPressedEvent;
    JzEvent<I32>     KeyboardButtonReleasedEvent;
    JzEvent<I32>     MouseButtonPressedEvent;
    JzEvent<I32>     MouseButtonReleasedEvent;
    JzEvent<JzIVec2> MouseMovedEvent;
    JzEvent<JzVec2>  MouseScrolledEvent;

    /* Window events */
    JzEvent<JzIVec2> WindowResizedEvent;
    JzEvent<JzIVec2> WindowFrameBufferResizedEvent;
    JzEvent<JzIVec2> WindowMoveEvent;
    JzEvent<JzIVec2> WindowCursorMoveEvent;
    JzEvent<>        WindowMinimizedEvent;
    JzEvent<>        WindowMaximizedEvent;
    JzEvent<>        WindowFocusGainEvent;
    JzEvent<>        WindowFocusLostEvent;
    JzEvent<>        WindowClosedEvent;

private:
    static std::unordered_map<GLFWwindow *, JzWindow *> __WINDOWS_MAP;

    JzERHIType  m_rhiType;
    GLFWwindow *m_glfwWindow;
    String      m_title;
    JzIVec2     m_size;
    JzIVec2     m_position;
    JzIVec2     m_minimumSize;
    JzIVec2     m_maximumSize;
    JzIVec2     m_windowedSize;
    JzIVec2     m_windowedPos;
    Bool        m_fullscreen;
    I32         m_refreshRate;
};

} // namespace JzRE
