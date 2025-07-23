#include "JzWindow.h"

JzRE::JzWindow::JzWindow(const JzDevice &device, const JzWindowSettings &windowSettings) :
    m_device(device) { }

JzRE::JzWindow::~JzWindow() { }

void JzRE::JzWindow::MakeCurrentContext() const
{
    glfwMakeContextCurrent(m_glfwWindow);
}

GLFWwindow *JzRE::JzWindow::GetGLFWWindow() const
{
    return m_glfwWindow;
}

void JzRE::JzWindow::SwapBuffers() const
{
    glfwSwapBuffers(m_glfwWindow);
}

JzRE::Bool JzRE::JzWindow::ShouldClose() const
{
    return glfwWindowShouldClose(m_glfwWindow);
}
