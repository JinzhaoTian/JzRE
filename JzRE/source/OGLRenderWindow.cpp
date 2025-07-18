#include "OGLRenderWindow.h"

namespace JzRE {
OGLRenderWindow::OGLRenderWindow(I32 width, I32 height, const String &title) :
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

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }

    glViewport(0, 0, this->wndWidth, this->wndHeight);
}

OGLRenderWindow::~OGLRenderWindow() {
    if (this->hwnd) {
        glfwDestroyWindow(this->hwnd);
        glfwTerminate();
    }
}

GLFWwindow* OGLRenderWindow::GetGLFWwindow() {
    return this->hwnd;
}

I32 OGLRenderWindow::GetWindowWidth() {
    return this->wndWidth;
}

I32 OGLRenderWindow::GetWindowHeight() {
    return this->wndHeight;
}

void OGLRenderWindow::ResizeWindow(I32 w, I32 h) {
    this->wndWidth = w;
    this->wndHeight = h;

    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, w, h);
}

Bool OGLRenderWindow::ShouldClose() {
    return glfwWindowShouldClose(this->hwnd);
}

void OGLRenderWindow::PollEvents() {
    glfwPollEvents();
}

void OGLRenderWindow::SwapFramebuffer() {
    glfwSwapBuffers(this->hwnd);
}

} // namespace JzRE