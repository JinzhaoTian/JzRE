#pragma once

#include "CommonTypes.h"
#include "JzDevice.h"
#include "JzEvent.h"
#include "JzWindowSettings.h"

namespace JzRE {
/**
 * @brief Window class
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
     * @brief Swap the buffers
     */
    void SwapBuffers() const;

    /**
     * @brief Check if the window should close
     *
     * @return Bool
     */
    Bool ShouldClose() const;

public:
    JzEvent<I32> KeyPressedEvent;
    JzEvent<I32> KeyReleasedEvent;
    JzEvent<I32> MouseButtonPressedEvent;
    JzEvent<I32> MouseButtonReleasedEvent;

    JzEvent<I32, I32> MouseMovedEvent;
    JzEvent<I32, I32> MouseScrolledEvent;

    JzEvent<I32, I32> WindowResizedEvent;
    JzEvent<I32, I32> FrameBufferResizedEvent;

    JzEvent<> WindowMinimizedEvent;
    JzEvent<> WindowMaximizedEvent;
    JzEvent<> WindowFocusGainEvent;
    JzEvent<> WindowFocusLostEvent;

    JzEvent<> WindowClosedEvent;

private:
    const JzDevice &m_device;
    GLFWwindow     *m_glfwWindow;
};
} // namespace JzRE
