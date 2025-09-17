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
#include "JzWindowSettings.h"
#include "JzRETypes.h"
#include "JzRHIETypes.h"
#include "JzEvent.h"
#include "JzVector.h"

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
     */
    void SetSize(JzIVec2 p_size);

    /**
     * @brief Get the current size of the window
     */
    JzIVec2 GetSize() const;

    /**
     * @brief Return true if the windows is minimized
     */
    Bool IsMinimized() const;

    /**
     * @brief Set a minimum size for the window
     */
    void SetMinimumSize(JzIVec2 minimumSize);

    /**
     * @brief Get the current minimum size of the window
     * @note -1 (WindowSettings::DontCare) values means no limitation
     */
    JzIVec2 GetMinimumSize() const;

    /**
     * @brief Return true if the windows is maximized
     */
    Bool IsMaximized() const;

    /**
     * @brief Set a maximum size for the window
     */
    void SetMaximumSize(JzIVec2 maximumSize);

    /**
     * @brief Get the current maximum size of the window
     * @note -1 (WindowSettings::DontCare) values means no limitation
     */
    JzIVec2 GetMaximumSize() const;

    /**
     * @brief Set the window in fullscreen mode
     */
    void SetFullscreen(Bool p_value);

    /**
     * @brief Return true if the window is fullscreen
     */
    Bool IsFullscreen() const;

    /**
     * @brief Set the window align centered
     */
    void SetAlignCentered();

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
    JzEvent<I32>     KeyPressedEvent;
    JzEvent<I32>     KeyReleasedEvent;
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
    JzIVec2     m_minimumSize;
    JzIVec2     m_maximumSize;
    JzIVec2     m_position;
    Bool        m_fullscreen;
    I32         m_refreshRate;
};

} // namespace JzRE
