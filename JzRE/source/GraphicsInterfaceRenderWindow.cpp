#include "GraphicsInterfaceRenderWindow.h"

namespace JzRE {

void callback_framebuffer_size(RawPtr<GLFWwindow> window, int width, int height) {
    auto self = StaticCast<RawPtr<GraphicsInterfaceRenderWindow>>(glfwGetWindowUserPointer(window));
    self->ResizeWindow(width, height);
}

GraphicsInterfaceRenderWindow::GraphicsInterfaceRenderWindow() :
    hwnd(nullptr), wndWidth(800), wndHeight(600), title("Graphics Interface Render Window") {
}

GraphicsInterfaceRenderWindow::~GraphicsInterfaceRenderWindow() {
    Shutdown();
}

Bool GraphicsInterfaceRenderWindow::Initialize(I32 w, I32 h, const String &title) {
    this->wndWidth = w;
    this->wndHeight = h;
    this->title = title;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    this->hwnd = glfwCreateWindow(w, h, title.c_str(), NULL, NULL);
    if (this->hwnd == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(this->hwnd);
    glfwSetWindowUserPointer(this->hwnd, this);

    // callback: frame buffer size
    glfwSetFramebufferSizeCallback(this->hwnd, callback_framebuffer_size);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return false;
    }

    glViewport(0, 0, w, h);

    return true;
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

void GraphicsInterfaceRenderWindow::Shutdown() {
    if (hwnd) {
        glfwDestroyWindow(hwnd);
        glfwTerminate();
    }
}

} // namespace JzRE