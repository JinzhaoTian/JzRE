#include "GraphicsInterfaceRenderWindow.h"

namespace JzRE {

void callback_framebuffer_size(RawPtr<GLFWwindow> window, int width, int height) {
    auto self = StaticCast<RawPtr<GraphicsInterfaceRenderWindow>>(glfwGetWindowUserPointer(window));
    self->ResizeWindow(width, height);
}

GraphicsInterfaceRenderWindow::GraphicsInterfaceRenderWindow(I32 width, I32 height, const String &title) :
    wndWidth(width), wndHeight(height), title(title) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    this->hwnd = glfwCreateWindow(this->wndWidth, this->wndHeight, title.c_str(), NULL, NULL);
    if (this->hwnd == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }

    glfwMakeContextCurrent(this->hwnd);
    glfwSetWindowUserPointer(this->hwnd, this);

    // callback: frame buffer size
    glfwSetFramebufferSizeCallback(this->hwnd, callback_framebuffer_size);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }

    glViewport(0, 0, this->wndWidth, this->wndHeight);
}

GraphicsInterfaceRenderWindow::~GraphicsInterfaceRenderWindow() {
    if (hwnd) {
        glfwDestroyWindow(hwnd);
        glfwTerminate();
    }
}

RawPtr<GLFWwindow> GraphicsInterfaceRenderWindow::GetGLFWwindow() {
    return hwnd;
}

void GraphicsInterfaceRenderWindow::ResizeWindow(I32 w, I32 h) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, w, h);
}

Bool GraphicsInterfaceRenderWindow::ShouldClose() {
    return glfwWindowShouldClose(this->hwnd);
}

void GraphicsInterfaceRenderWindow::PollEvents() {
    glfwPollEvents();
}

void GraphicsInterfaceRenderWindow::SwapFramebuffer() {
    glfwSwapBuffers(this->hwnd);
}

} // namespace JzRE