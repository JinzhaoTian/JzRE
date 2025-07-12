#include "SoftwareRenderWindow.h"

namespace JzRE {

SoftwareRenderWindow::SoftwareRenderWindow() :
    window(nullptr), wndWidth(800), wndHeight(600), title("Software/CPU Render Engine") {
}

SoftwareRenderWindow::~SoftwareRenderWindow() {
    if (window) {
        glfwDestroyWindow(window);
    }
}

Bool SoftwareRenderWindow::Initialize(I32 w, I32 h, const String &title) {
    this->wndWidth = w;
    this->wndHeight = h;
    this->hasClosed = false;

    frontBuffer = CreateSharedPtr<Framebuffer>(wndWidth, wndHeight);
    backBuffer = CreateSharedPtr<Framebuffer>(wndWidth, wndHeight);

    // 初始化GLFW
    if (!glfwInit()) {
        return false;
    }

    // 创建窗口
    window = glfwCreateWindow(wndWidth, wndHeight, title.c_str(), nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return false;
    }

    // 设置窗口关闭回调
    glfwSetWindowCloseCallback(window, [](GLFWwindow* w) {
        glfwSetWindowShouldClose(w, GLFW_TRUE);
    });

    // 设置窗口大小改变回调
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* w, int width, int height) {
        SoftwareRenderWindow* window = static_cast<SoftwareRenderWindow*>(glfwGetWindowUserPointer(w));
        if (window) {
            window->ResizeWindow(width, height);
        }
    });

    glfwSetWindowUserPointer(window, this);

    return true;
}

Bool SoftwareRenderWindow::ShouldClose() {
    return glfwWindowShouldClose(window);
}

void SoftwareRenderWindow::PollEvents() {
    glfwPollEvents();
}

void SoftwareRenderWindow::SwapBuffer() {
    // 将后缓冲区的数据复制到前缓冲区
    memcpy(frontBuffer->data, backBuffer->data, wndWidth * wndHeight * sizeof(U32));

    // 使用OpenGL渲染到窗口
    glfwMakeContextCurrent(window);
    
    // 清除屏幕
    glClear(GL_COLOR_BUFFER_BIT);
    
    // 这里可以添加OpenGL渲染代码来显示framebuffer的内容
    // 由于这是软件渲染引擎，我们暂时只交换缓冲区
    
    glfwSwapBuffers(window);
}

void SoftwareRenderWindow::ClearBuffer() {
    frontBuffer->Clear();
    backBuffer->Clear();
}

void SoftwareRenderWindow::ResizeWindow(I32 w, I32 h) {
    wndWidth = w;
    wndHeight = h;

    frontBuffer->Resize(w, h);
    backBuffer->Resize(w, h);
}

} // namespace JzRE