/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Function/Window/JzWindow.h"
#include "JzRE/Runtime/Core/JzEvent.h"
#include <stdexcept>
#if defined(_WIN32)
#define GLFW_EXPOSE_NATIVE_WIN32
#elif defined(__APPLE__)
#define GLFW_EXPOSE_NATIVE_COCOA
#elif defined(__linux__)
#define GLFW_EXPOSE_NATIVE_X11
#endif
#include <GLFW/glfw3native.h>

std::unordered_map<GLFWwindow *, JzRE::JzWindow *> JzRE::JzWindow::__WINDOWS_MAP;

JzRE::JzWindow::JzWindow(JzRE::JzERHIType rhiType, const JzRE::JzWindowSettings &windowSettings) :
    m_rhiType(rhiType),
    m_title(windowSettings.title),
    m_size(windowSettings.size),
    m_position(windowSettings.position),
    m_windowedSize(windowSettings.size),
    m_windowedPos(windowSettings.position),
    m_minimumSize(windowSettings.minSize),
    m_maximumSize(windowSettings.maxSize),
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

    WindowResizedEvent.AddListener(std::bind(&JzWindow::OnResize, this, std::placeholders::_1));

    WindowMoveEvent.AddListener(std::bind(&JzWindow::OnMove, this, std::placeholders::_1));
}

JzRE::JzWindow::~JzWindow()
{
    glfwDestroyWindow(m_glfwWindow);
}

void JzRE::JzWindow::MakeCurrentContext() const
{
    glfwMakeContextCurrent(m_glfwWindow);
}

void JzRE::JzWindow::DetachContext() const
{
    glfwMakeContextCurrent(nullptr);
}

GLFWwindow *JzRE::JzWindow::GetGLFWWindow() const
{
    return m_glfwWindow;
}

void *JzRE::JzWindow::GetNativeWindow() const
{
#if defined(_WIN32)
    return glfwGetWin32Window(m_glfwWindow); // HWND
#elif defined(__APPLE__)
    return glfwGetCocoaWindow(m_glfwWindow); // NSWindow*
#elif defined(__linux__)
    return glfwGetX11Window(m_glfwWindow);
#endif
}

JzRE::JzIVec2 JzRE::JzWindow::GetMonitorSize() const
{
    const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    return {static_cast<I32>(mode->width), static_cast<I32>(mode->height)};
}

void JzRE::JzWindow::SetTitle(const JzRE::String &title)
{
    m_title = title;
    glfwSetWindowTitle(m_glfwWindow, title.c_str());
}

JzRE::String JzRE::JzWindow::GetTitle() const
{
    return m_title;
}

void JzRE::JzWindow::SetPosition(JzRE::JzIVec2 p_position)
{
    if (!m_fullscreen) {
        glfwSetWindowPos(m_glfwWindow, p_position.x, p_position.y);
        m_position = p_position;
    }
}

JzRE::JzIVec2 JzRE::JzWindow::GetPosition() const
{
    I32 x, y;
    glfwGetWindowPos(m_glfwWindow, &x, &y);
    return {x, y};
}

void JzRE::JzWindow::SetSize(JzRE::JzIVec2 p_size)
{
    if (!m_fullscreen) {
        glfwSetWindowSize(m_glfwWindow, p_size.x, p_size.y);
        m_size = p_size;
    }
}

JzRE::JzIVec2 JzRE::JzWindow::GetSize() const
{
    I32 width, height;
    glfwGetWindowSize(m_glfwWindow, &width, &height);
    return {width, height};
}

JzRE::JzIVec2 JzRE::JzWindow::GetFramebufferSize() const
{
    I32 width, height;
    glfwGetFramebufferSize(m_glfwWindow, &width, &height);
    return {width, height};
}

JzRE::Bool JzRE::JzWindow::IsMinimized() const
{
    return glfwGetWindowAttrib(m_glfwWindow, GLFW_ICONIFIED) == GLFW_TRUE;
}

void JzRE::JzWindow::Minimize() const
{
    glfwIconifyWindow(m_glfwWindow);
}

void JzRE::JzWindow::Restore() const
{
    glfwRestoreWindow(m_glfwWindow);
}

void JzRE::JzWindow::SetMinimumSize(JzRE::JzIVec2 minimumSize)
{
    m_minimumSize = minimumSize;

    UpdateSizeLimit();
}

JzRE::JzIVec2 JzRE::JzWindow::GetMinimumSize() const
{
    return m_minimumSize;
}

JzRE::Bool JzRE::JzWindow::IsMaximized() const
{
    return glfwGetWindowAttrib(m_glfwWindow, GLFW_MAXIMIZED) == GLFW_TRUE;
}

void JzRE::JzWindow::Maximize() const
{
    glfwMaximizeWindow(m_glfwWindow);
}

void JzRE::JzWindow::SetMaximumSize(JzRE::JzIVec2 maximumSize)
{
    m_maximumSize = maximumSize;

    UpdateSizeLimit();
}

JzRE::JzIVec2 JzRE::JzWindow::GetMaximumSize() const
{
    return m_maximumSize;
}

void JzRE::JzWindow::SetFullscreen(JzRE::Bool value)
{
    if (value) {
        m_fullscreen   = true;
        m_windowedSize = m_size;
        m_windowedPos  = m_position;

        glfwSetWindowMonitor(
            m_glfwWindow,
            glfwGetPrimaryMonitor(),
            static_cast<I32>(m_position.x),
            static_cast<I32>(m_position.y),
            static_cast<I32>(m_size.x),
            static_cast<I32>(m_size.y),
            m_refreshRate);
    } else {
        m_fullscreen = false;
        m_size       = m_windowedSize;
        m_position   = m_windowedPos;

        glfwSetWindowMonitor(
            m_glfwWindow,
            nullptr,
            static_cast<I32>(m_position.x),
            static_cast<I32>(m_position.y),
            static_cast<I32>(m_size.x),
            static_cast<I32>(m_size.y),
            m_refreshRate);
    }
}

JzRE::Bool JzRE::JzWindow::IsFullscreen() const
{
    return glfwGetWindowMonitor(m_glfwWindow) != nullptr;
}

void JzRE::JzWindow::SetAlignCentered()
{
    const GLFWvidmode *mode      = glfwGetVideoMode(glfwGetPrimaryMonitor());
    auto               monWidth  = mode->width;
    auto               monHeight = mode->height;

    I32 winWidth, winHeight;
    glfwGetWindowSize(m_glfwWindow, &winWidth, &winHeight);

    glfwSetWindowPos(m_glfwWindow, monWidth / 2 - winWidth / 2, monHeight / 2 - winHeight / 2);
}

JzRE::Bool JzRE::JzWindow::IsHidden() const
{
    return glfwGetWindowAttrib(m_glfwWindow, GLFW_VISIBLE) == GLFW_FALSE;
}

JzRE::Bool JzRE::JzWindow::IsVisible() const
{
    return glfwGetWindowAttrib(m_glfwWindow, GLFW_VISIBLE) == GLFW_TRUE;
}

void JzRE::JzWindow::Hide() const
{
    glfwHideWindow(m_glfwWindow);
}

void JzRE::JzWindow::Show() const
{
    glfwShowWindow(m_glfwWindow);
}

JzRE::Bool JzRE::JzWindow::IsFocused() const
{
    return glfwGetWindowAttrib(m_glfwWindow, GLFW_FOCUSED) == GLFW_TRUE;
}

void JzRE::JzWindow::Focus() const
{
    glfwFocusWindow(m_glfwWindow);
}

void JzRE::JzWindow::PollEvents() const
{
    glfwPollEvents();
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
    I32 initCode = glfwInit();
    if (initCode == GLFW_FALSE) {
        throw std::runtime_error("Failed to Init GLFW");
        glfwTerminate();
    }

    glfwDefaultWindowHints();

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

    switch (m_rhiType) {
        case JzERHIType::Unknown:
            break;

        case JzERHIType::OpenGL:
            glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
            break;

        case JzERHIType::Vulkan:
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            break;

        case JzERHIType::D3D11:
            break;

        case JzERHIType::D3D12:
            break;

        case JzERHIType::Metal:
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#ifdef __APPLE__
            // macOS特定：启用透明框架以支持Metal层
            glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
#endif
            break;
    }

    m_glfwWindow = glfwCreateWindow(
        m_size.x,
        m_size.y,
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
            m_minimumSize.x,
            m_minimumSize.y,
            m_maximumSize.x,
            m_maximumSize.y);

        glfwSetWindowPos(
            m_glfwWindow,
            m_position.x,
            m_position.y);

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
                windowInstance->KeyboardButtonPressedEvent.Invoke(p_key);

            if (p_action == GLFW_RELEASE)
                windowInstance->KeyboardButtonReleasedEvent.Invoke(p_key);
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
            windowInstance->MouseScrolledEvent.Invoke({static_cast<F32>(p_xOffset), static_cast<F32>(p_yOffset)});
        }
    };

    glfwSetScrollCallback(m_glfwWindow, scrollCallback);
}

void JzRE::JzWindow::BindResizeCallback() const
{
    auto resizeCallback = [](GLFWwindow *p_window, I32 p_width, I32 p_height) {
        JzWindow *windowInstance = FindInstance(p_window);

        if (windowInstance) {
            windowInstance->WindowResizedEvent.Invoke({p_width, p_height});
        }
    };

    glfwSetWindowSizeCallback(m_glfwWindow, resizeCallback);
}

void JzRE::JzWindow::BindFramebufferResizeCallback() const
{
    auto framebufferResizeCallback = [](GLFWwindow *p_window, I32 p_width, I32 p_height) {
        JzWindow *windowInstance = FindInstance(p_window);

        if (windowInstance) {
            windowInstance->WindowFrameBufferResizedEvent.Invoke({p_width, p_height});
        }
    };

    glfwSetFramebufferSizeCallback(m_glfwWindow, framebufferResizeCallback);
}

void JzRE::JzWindow::BindCursorMoveCallback() const
{
    auto cursorMoveCallback = [](GLFWwindow *p_window, F64 p_x, F64 p_y) {
        JzWindow *windowInstance = FindInstance(p_window);

        if (windowInstance) {
            windowInstance->WindowCursorMoveEvent.Invoke({static_cast<I32>(p_x), static_cast<I32>(p_y)});
        }
    };

    glfwSetCursorPosCallback(m_glfwWindow, cursorMoveCallback);
}

void JzRE::JzWindow::BindMoveCallback() const
{
    auto moveCallback = [](GLFWwindow *p_window, I32 p_x, I32 p_y) {
        JzWindow *windowInstance = FindInstance(p_window);

        if (windowInstance) {
            windowInstance->WindowMoveEvent.Invoke({p_x, p_y});
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

void JzRE::JzWindow::OnResize(JzRE::JzIVec2 p_size)
{
    m_size = p_size;
}

void JzRE::JzWindow::OnMove(JzRE::JzIVec2 p_position)
{
    if (!m_fullscreen) {
        m_position = p_position;
    }
}

void JzRE::JzWindow::UpdateSizeLimit() const
{
    glfwSetWindowSizeLimits(
        m_glfwWindow,
        m_minimumSize.x,
        m_minimumSize.y,
        m_maximumSize.x,
        m_maximumSize.y);
}