#include "GraphicsInterfaceRenderWindow.h"

namespace JzRE {
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

GraphicsInterfaceRenderWindow::GraphicsInterfaceRenderWindow(I32 w, I32 h, const String &title) :
    wndWidth(w), wndHeight(h) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    this->hwnd = glfwCreateWindow(w, h, title.c_str(), NULL, NULL);
    if (this->hwnd == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(this->hwnd);
    glfwSetFramebufferSizeCallback(this->hwnd, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        this->hwnd == NULL;
        return;
    }
}

void GraphicsInterfaceRenderWindow::ResizeWindow(I32 w, I32 h) {
    glViewport(0, 0, w, h);
}

bool GraphicsInterfaceRenderWindow::ShouldClose() {
    return glfwWindowShouldClose(this->hwnd);
}

void GraphicsInterfaceRenderWindow::PollEvents() {
    glfwPollEvents();
}

void GraphicsInterfaceRenderWindow::ProcessInput() {
    if (glfwGetKey(this->hwnd, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(this->hwnd, true);
}

void GraphicsInterfaceRenderWindow::SwapBuffer() {
    glfwSwapBuffers(this->hwnd);
}

void GraphicsInterfaceRenderWindow::ClearBuffer() {
}

} // namespace JzRE