#pragma once

#include "CommonTypes.h"
#include "JzDevice.h"
#include "JzEvent.h"
#include "JzWindowSettings.h"

namespace JzRE {
/**
 * @brief Window of JzRE
 */
class JzWindow {
public:
    /**
     * @brief Constructor
     */
    JzWindow(const JzDevice &device, const JzWindowSettings &windowSettings);

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
     * @brief Set the title of the window
     *
     * @param title
     */
    void SetTitle(const String &title);

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

    void OnResize(U16 width, U16 height);
    void OnMove(I16 x, I16 y);

public:
    /* Inputs events */
    JzEvent<I32>      KeyPressedEvent;
    JzEvent<I32>      KeyReleasedEvent;
    JzEvent<I32>      MouseButtonPressedEvent;
    JzEvent<I32>      MouseButtonReleasedEvent;
    JzEvent<U16, U16> MouseMovedEvent;
    JzEvent<F64, F64> MouseScrolledEvent;

    /* Window events */
    JzEvent<U16, U16> WindowResizedEvent;
    JzEvent<U16, U16> WindowFrameBufferResizedEvent;
    JzEvent<I16, I16> WindowMoveEvent;
    JzEvent<I16, I16> WindowCursorMoveEvent;
    JzEvent<>         WindowMinimizedEvent;
    JzEvent<>         WindowMaximizedEvent;
    JzEvent<>         WindowFocusGainEvent;
    JzEvent<>         WindowFocusLostEvent;
    JzEvent<>         WindowClosedEvent;

private:
    static std::unordered_map<GLFWwindow *, JzWindow *> __WINDOWS_MAP;

    const JzDevice     &m_device;
    GLFWwindow         *m_glfwWindow;
    String              m_title;
    std::pair<U16, U16> m_size;
    std::pair<I16, I16> m_minimumSize;
    std::pair<I16, I16> m_maximumSize;
    std::pair<I16, I16> m_position;
    Bool                m_fullscreen;
    I32                 m_refreshRate;
};
} // namespace JzRE
