#include "JzWindow.h"

std::unordered_map<GLFWwindow *, JzRE::JzWindow *> JzRE::JzWindow::__WINDOWS_MAP;

JzRE::JzWindow::JzWindow(const JzDevice &device, const JzWindowSettings &windowSettings) :
    m_device(device),
    m_title(windowSettings.title),
    m_size{windowSettings.width, windowSettings.height},
    m_position{windowSettings.x, windowSettings.y},
    m_minimumSize{windowSettings.minWidth, windowSettings.minHeight},
    m_maximumSize{windowSettings.maxWidth, windowSettings.maxHeight},
    m_fullscreen(windowSettings.isFullscreen),
    m_refreshRate(windowSettings.refreshRate)
{
    CreateGlfwWindow(windowSettings);

    BindKeyCallback();
    BindMouseCallback();
    BindScrollCallback();
    BindIconifyCallback();
    BindCloseCallback();
    BindResizeCallback();
    BindCursorMoveCallback();
    BindFramebufferResizeCallback();
    BindMoveCallback();
    BindFocusCallback();

    WindowResizedEvent.AddListener(std::bind(&JzWindow::OnResize, this, std::placeholders::_1, std::placeholders::_2));

    WindowMoveEvent.AddListener(std::bind(&JzWindow::OnMove, this, std::placeholders::_1, std::placeholders::_2));
}

JzRE::JzWindow::~JzWindow()
{
    glfwDestroyWindow(m_glfwWindow);
}

void JzRE::JzWindow::MakeCurrentContext() const
{
    glfwMakeContextCurrent(m_glfwWindow);
}

GLFWwindow *JzRE::JzWindow::GetGLFWWindow() const
{
    return m_glfwWindow;
}

void JzRE::JzWindow::SetTitle(const JzRE::String &title)
{
    m_title = title;
    glfwSetWindowTitle(m_glfwWindow, title.c_str());
}

void JzRE::JzWindow::SwapBuffers() const
{
    glfwSwapBuffers(m_glfwWindow);
}

void JzRE::JzWindow::SetShouldClose(JzRE::Bool value) const
{
    glfwSetWindowShouldClose(m_glfwWindow, value);
}

JzRE::Bool JzRE::JzWindow::ShouldClose() const
{
    return glfwWindowShouldClose(m_glfwWindow);
}

void JzRE::JzWindow::CreateGlfwWindow(const JzRE::JzWindowSettings &windowSettings)
{
    GLFWmonitor *selectedMonitor = nullptr;

    if (m_fullscreen)
        selectedMonitor = glfwGetPrimaryMonitor();

    glfwWindowHint(GLFW_RESIZABLE, windowSettings.isResizable);
    glfwWindowHint(GLFW_DECORATED, windowSettings.isDecorated);
    glfwWindowHint(GLFW_FOCUSED, windowSettings.isFocused);
    glfwWindowHint(GLFW_MAXIMIZED, windowSettings.isMaximized);
    glfwWindowHint(GLFW_FLOATING, windowSettings.isFloating);
    glfwWindowHint(GLFW_VISIBLE, windowSettings.isVisible);
    glfwWindowHint(GLFW_AUTO_ICONIFY, windowSettings.autoIconify);
    glfwWindowHint(GLFW_SAMPLES, windowSettings.samples);

    m_glfwWindow = glfwCreateWindow(
        static_cast<I32>(m_size.first),
        static_cast<I32>(m_size.second),
        m_title.c_str(),
        selectedMonitor,
        nullptr);

    if (!m_glfwWindow) {
        const char *description;
        I32         error_code    = glfwGetError(&description);
        String      error_message = "Failed to create GLFW window";
        if (error_code != GLFW_NO_ERROR) {
            error_message += " - GLFW Error " + std::to_string(error_code) + ": " + std::string(description);
        }

        throw std::runtime_error(error_message);
    } else {
        glfwSetWindowSizeLimits(
            m_glfwWindow,
            static_cast<I32>(m_minimumSize.first),
            static_cast<I32>(m_minimumSize.second),
            static_cast<I32>(m_maximumSize.first),
            static_cast<I32>(m_maximumSize.second));

        glfwSetWindowPos(
            m_glfwWindow,
            static_cast<I32>(m_position.first),
            static_cast<I32>(m_position.second));

        __WINDOWS_MAP[m_glfwWindow] = this;
    }
}

JzRE::JzWindow *JzRE::JzWindow::FindInstance(GLFWwindow *p_glfwWindow)
{
    return __WINDOWS_MAP.find(p_glfwWindow) != __WINDOWS_MAP.end() ? __WINDOWS_MAP[p_glfwWindow] : nullptr;
}

void JzRE::JzWindow::BindKeyCallback() const
{
    auto keyCallback = [](GLFWwindow *p_window, I32 p_key, I32 p_scancode, I32 p_action, I32 p_mods) {
        JzWindow *windowInstance = FindInstance(p_window);

        if (windowInstance) {
            if (p_action == GLFW_PRESS)
                windowInstance->KeyPressedEvent.Invoke(p_key);

            if (p_action == GLFW_RELEASE)
                windowInstance->KeyReleasedEvent.Invoke(p_key);
        }
    };

    glfwSetKeyCallback(m_glfwWindow, keyCallback);
}

void JzRE::JzWindow::BindMouseCallback() const
{
    auto mouseCallback = [](GLFWwindow *p_window, I32 p_button, I32 p_action, I32 p_mods) {
        JzWindow *windowInstance = FindInstance(p_window);

        if (windowInstance) {
            if (p_action == GLFW_PRESS)
                windowInstance->MouseButtonPressedEvent.Invoke(p_button);

            if (p_action == GLFW_RELEASE)
                windowInstance->MouseButtonReleasedEvent.Invoke(p_button);
        }
    };

    glfwSetMouseButtonCallback(m_glfwWindow, mouseCallback);
}

void JzRE::JzWindow::BindScrollCallback() const
{
    auto scrollCallback = [](GLFWwindow *p_window, F64 p_xOffset, F64 p_yOffset) {
        JzWindow *windowInstance = FindInstance(p_window);

        if (windowInstance) {
            windowInstance->MouseMovedEvent.Invoke(p_xOffset, p_yOffset);
        }
    };

    glfwSetScrollCallback(m_glfwWindow, scrollCallback);
}

void JzRE::JzWindow::BindResizeCallback() const
{
    auto resizeCallback = [](GLFWwindow *p_window, I32 p_width, I32 p_height) {
        JzWindow *windowInstance = FindInstance(p_window);

        if (windowInstance) {
            windowInstance->WindowResizedEvent.Invoke(static_cast<U16>(p_width), static_cast<U16>(p_height));
        }
    };

    glfwSetWindowSizeCallback(m_glfwWindow, resizeCallback);
}

void JzRE::JzWindow::BindFramebufferResizeCallback() const
{
    auto framebufferResizeCallback = [](GLFWwindow *p_window, I32 p_width, I32 p_height) {
        JzWindow *windowInstance = FindInstance(p_window);

        if (windowInstance) {
            windowInstance->WindowFrameBufferResizedEvent.Invoke(static_cast<U16>(p_width), static_cast<U16>(p_height));
        }
    };

    glfwSetFramebufferSizeCallback(m_glfwWindow, framebufferResizeCallback);
}

void JzRE::JzWindow::BindCursorMoveCallback() const
{
    auto cursorMoveCallback = [](GLFWwindow *p_window, F64 p_x, F64 p_y) {
        JzWindow *windowInstance = FindInstance(p_window);

        if (windowInstance) {
            windowInstance->WindowCursorMoveEvent.Invoke(static_cast<I16>(p_x), static_cast<I16>(p_y));
        }
    };

    glfwSetCursorPosCallback(m_glfwWindow, cursorMoveCallback);
}

void JzRE::JzWindow::BindMoveCallback() const
{
    auto moveCallback = [](GLFWwindow *p_window, I32 p_x, I32 p_y) {
        JzWindow *windowInstance = FindInstance(p_window);

        if (windowInstance) {
            windowInstance->WindowMoveEvent.Invoke(static_cast<I16>(p_x), static_cast<I16>(p_y));
        }
    };

    glfwSetWindowPosCallback(m_glfwWindow, moveCallback);
}

void JzRE::JzWindow::BindIconifyCallback() const
{
    auto iconifyCallback = [](GLFWwindow *p_window, I32 p_iconified) {
        JzWindow *windowInstance = FindInstance(p_window);

        if (windowInstance) {
            if (p_iconified == GLFW_TRUE)
                windowInstance->WindowMinimizedEvent.Invoke();

            if (p_iconified == GLFW_FALSE)
                windowInstance->WindowMaximizedEvent.Invoke();
        }
    };

    glfwSetWindowIconifyCallback(m_glfwWindow, iconifyCallback);
}

void JzRE::JzWindow::BindFocusCallback() const
{
    auto focusCallback = [](GLFWwindow *p_window, I32 p_focused) {
        JzWindow *windowInstance = FindInstance(p_window);

        if (windowInstance) {
            if (p_focused == GLFW_TRUE)
                windowInstance->WindowFocusGainEvent.Invoke();

            if (p_focused == GLFW_FALSE)
                windowInstance->WindowFocusLostEvent.Invoke();
        }
    };

    glfwSetWindowFocusCallback(m_glfwWindow, focusCallback);
}

void JzRE::JzWindow::BindCloseCallback() const
{
    auto closeCallback = [](GLFWwindow *p_window) {
        JzWindow *windowInstance = FindInstance(p_window);

        if (windowInstance) {
            windowInstance->WindowClosedEvent.Invoke();
        }
    };

    glfwSetWindowCloseCallback(m_glfwWindow, closeCallback);
}

void JzRE::JzWindow::OnResize(JzRE::U16 width, JzRE::U16 height)
{
    m_size.first  = width;
    m_size.second = height;
}

void JzRE::JzWindow::OnMove(JzRE::I16 x, JzRE::I16 y)
{
    if (!m_fullscreen) {
        m_position.first  = x;
        m_position.second = y;
    }
}