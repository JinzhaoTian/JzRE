/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Platform/Window/JzGLFWWindowBackend.h"

#include <stdexcept>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif

#include <GLFW/glfw3.h>

#if defined(_WIN32)
#define GLFW_EXPOSE_NATIVE_WIN32
#elif defined(__APPLE__)
#define GLFW_EXPOSE_NATIVE_COCOA
#elif defined(__linux__)
#define GLFW_EXPOSE_NATIVE_X11
#endif
#include <GLFW/glfw3native.h>

namespace JzRE {

// ==================== Lifecycle ====================

JzGLFWWindowBackend::~JzGLFWWindowBackend()
{
    Shutdown();
}

void JzGLFWWindowBackend::Initialize(JzERHIType rhiType, const JzWindowConfig &config)
{
    m_rhiType      = rhiType;
    m_title        = config.title;
    m_size         = JzIVec2(config.width, config.height);
    m_windowedSize = m_size;
    m_fullscreen   = config.fullscreen;

    CreateGLFWWindow(config);
    SetupCallbacks();
}

void JzGLFWWindowBackend::Shutdown()
{
    if (m_window) {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
}

Bool JzGLFWWindowBackend::IsValid() const
{
    return m_window != nullptr;
}

// ==================== Context Management ====================

void JzGLFWWindowBackend::MakeContextCurrent()
{
    if (m_window) {
        glfwMakeContextCurrent(m_window);
    }
}

void JzGLFWWindowBackend::DetachContext()
{
    glfwMakeContextCurrent(nullptr);
}

void JzGLFWWindowBackend::SwapBuffers()
{
    if (m_window) {
        glfwSwapBuffers(m_window);
    }
}

// ==================== Event Polling ====================

void JzGLFWWindowBackend::PollEvents()
{
    glfwPollEvents();
}

// ==================== Window Properties ====================

void JzGLFWWindowBackend::SetTitle(const String &title)
{
    m_title = title;
    if (m_window) {
        glfwSetWindowTitle(m_window, title.c_str());
    }
}

String JzGLFWWindowBackend::GetTitle() const
{
    return m_title;
}

void JzGLFWWindowBackend::SetPosition(JzIVec2 position)
{
    if (!m_fullscreen && m_window) {
        glfwSetWindowPos(m_window, position.x, position.y);
        m_position = position;
    }
}

JzIVec2 JzGLFWWindowBackend::GetPosition() const
{
    if (!m_window) return m_position;
    I32 x, y;
    glfwGetWindowPos(m_window, &x, &y);
    return {x, y};
}

void JzGLFWWindowBackend::SetSize(JzIVec2 size)
{
    if (!m_fullscreen && m_window) {
        glfwSetWindowSize(m_window, size.x, size.y);
        m_size = size;
    }
}

JzIVec2 JzGLFWWindowBackend::GetSize() const
{
    if (!m_window) return m_size;
    I32 width, height;
    glfwGetWindowSize(m_window, &width, &height);
    return {width, height};
}

JzIVec2 JzGLFWWindowBackend::GetFramebufferSize() const
{
    if (!m_window) return m_size;
    I32 width, height;
    glfwGetFramebufferSize(m_window, &width, &height);
    return {width, height};
}

JzIVec2 JzGLFWWindowBackend::GetMonitorSize() const
{
    const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    return {static_cast<I32>(mode->width), static_cast<I32>(mode->height)};
}

void JzGLFWWindowBackend::SetMinimumSize(JzIVec2 size)
{
    m_minimumSize = size;
    UpdateSizeLimit();
}

JzIVec2 JzGLFWWindowBackend::GetMinimumSize() const
{
    return m_minimumSize;
}

void JzGLFWWindowBackend::SetMaximumSize(JzIVec2 size)
{
    m_maximumSize = size;
    UpdateSizeLimit();
}

JzIVec2 JzGLFWWindowBackend::GetMaximumSize() const
{
    return m_maximumSize;
}

// ==================== Window State ====================

void JzGLFWWindowBackend::SetFullscreen(Bool value)
{
    if (!m_window) return;

    if (value) {
        m_fullscreen   = true;
        m_windowedSize = m_size;
        m_windowedPos  = m_position;

        glfwSetWindowMonitor(
            m_window,
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
            m_window,
            nullptr,
            static_cast<I32>(m_position.x),
            static_cast<I32>(m_position.y),
            static_cast<I32>(m_size.x),
            static_cast<I32>(m_size.y),
            m_refreshRate);
    }
}

Bool JzGLFWWindowBackend::IsFullscreen() const
{
    if (!m_window) return false;
    return glfwGetWindowMonitor(m_window) != nullptr;
}

void JzGLFWWindowBackend::SetAlignCentered()
{
    if (!m_window) return;

    const GLFWvidmode *mode      = glfwGetVideoMode(glfwGetPrimaryMonitor());
    auto               monWidth  = mode->width;
    auto               monHeight = mode->height;

    I32 winWidth, winHeight;
    glfwGetWindowSize(m_window, &winWidth, &winHeight);

    glfwSetWindowPos(m_window, monWidth / 2 - winWidth / 2, monHeight / 2 - winHeight / 2);
}

Bool JzGLFWWindowBackend::IsMinimized() const
{
    if (!m_window) return false;
    return glfwGetWindowAttrib(m_window, GLFW_ICONIFIED) == GLFW_TRUE;
}

void JzGLFWWindowBackend::Minimize()
{
    if (m_window) {
        glfwIconifyWindow(m_window);
    }
}

void JzGLFWWindowBackend::Restore()
{
    if (m_window) {
        glfwRestoreWindow(m_window);
    }
}

Bool JzGLFWWindowBackend::IsMaximized() const
{
    if (!m_window) return false;
    return glfwGetWindowAttrib(m_window, GLFW_MAXIMIZED) == GLFW_TRUE;
}

void JzGLFWWindowBackend::Maximize()
{
    if (m_window) {
        glfwMaximizeWindow(m_window);
    }
}

Bool JzGLFWWindowBackend::IsHidden() const
{
    if (!m_window) return false;
    return glfwGetWindowAttrib(m_window, GLFW_VISIBLE) == GLFW_FALSE;
}

Bool JzGLFWWindowBackend::IsVisible() const
{
    if (!m_window) return false;
    return glfwGetWindowAttrib(m_window, GLFW_VISIBLE) == GLFW_TRUE;
}

void JzGLFWWindowBackend::Hide()
{
    if (m_window) {
        glfwHideWindow(m_window);
    }
}

void JzGLFWWindowBackend::Show()
{
    if (m_window) {
        glfwShowWindow(m_window);
    }
}

Bool JzGLFWWindowBackend::IsFocused() const
{
    if (!m_window) return false;
    return glfwGetWindowAttrib(m_window, GLFW_FOCUSED) == GLFW_TRUE;
}

void JzGLFWWindowBackend::Focus()
{
    if (m_window) {
        glfwFocusWindow(m_window);
    }
}

void JzGLFWWindowBackend::SetShouldClose(Bool value)
{
    if (m_window) {
        glfwSetWindowShouldClose(m_window, value);
    }
}

Bool JzGLFWWindowBackend::ShouldClose() const
{
    if (!m_window) return true;
    return glfwWindowShouldClose(m_window);
}

// ==================== Input Polling ====================

Bool JzGLFWWindowBackend::GetKeyState(I32 key) const
{
    if (!m_window) return false;
    return glfwGetKey(m_window, key) == GLFW_PRESS;
}

Bool JzGLFWWindowBackend::GetMouseButtonState(I32 button) const
{
    if (!m_window) return false;
    return glfwGetMouseButton(m_window, button) == GLFW_PRESS;
}

JzVec2 JzGLFWWindowBackend::GetCursorPosition() const
{
    if (!m_window) return JzVec2(0.0f, 0.0f);
    F64 x, y;
    glfwGetCursorPos(m_window, &x, &y);
    return JzVec2(static_cast<F32>(x), static_cast<F32>(y));
}

// ==================== Platform Handles ====================

void *JzGLFWWindowBackend::GetPlatformWindowHandle() const
{
    return static_cast<void *>(m_window);
}

void *JzGLFWWindowBackend::GetNativeWindowHandle() const
{
    if (!m_window) return nullptr;

#if defined(_WIN32)
    return glfwGetWin32Window(m_window);
#elif defined(__APPLE__)
    return glfwGetCocoaWindow(m_window);
#elif defined(__linux__)
    return reinterpret_cast<void *>(glfwGetX11Window(m_window));
#else
    return nullptr;
#endif
}

// ==================== Private: Window Creation ====================

void JzGLFWWindowBackend::CreateGLFWWindow(const JzWindowConfig &config)
{
    I32 initCode = glfwInit();
    if (initCode == GLFW_FALSE) {
        glfwTerminate();
        throw std::runtime_error("Failed to Init GLFW");
    }

    glfwDefaultWindowHints();

    GLFWmonitor *selectedMonitor = nullptr;
    if (m_fullscreen) {
        selectedMonitor = glfwGetPrimaryMonitor();
    }

    glfwWindowHint(GLFW_RESIZABLE, config.resizable);
    glfwWindowHint(GLFW_DECORATED, config.decorated);
    glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_FALSE);
    glfwWindowHint(GLFW_FLOATING, config.floating);
    glfwWindowHint(GLFW_VISIBLE, config.visible);
    glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE);
    glfwWindowHint(GLFW_SAMPLES, config.samples);

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
            glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
#endif
            break;
    }

    m_window = glfwCreateWindow(
        m_size.x,
        m_size.y,
        m_title.c_str(),
        selectedMonitor,
        nullptr);

    if (!m_window) {
        const char *description;
        I32         error_code    = glfwGetError(&description);
        String      error_message = "Failed to create GLFW window";
        if (error_code != GLFW_NO_ERROR) {
            error_message += " - GLFW Error " + std::to_string(error_code) + ": " + std::string(description);
        }
        throw std::runtime_error(error_message);
    }

    glfwSetWindowSizeLimits(
        m_window,
        m_minimumSize.x,
        m_minimumSize.y,
        m_maximumSize.x,
        m_maximumSize.y);

    glfwSetWindowPos(
        m_window,
        m_position.x,
        m_position.y);
}

void JzGLFWWindowBackend::SetupCallbacks()
{
    if (!m_window) return;

    // Store this pointer for lambda access
    glfwSetWindowUserPointer(m_window, this);

    // Key callback
    glfwSetKeyCallback(m_window, [](GLFWwindow *window, I32 key, I32 /*scancode*/, I32 action, I32 /*mods*/) {
        auto *self = static_cast<JzGLFWWindowBackend *>(glfwGetWindowUserPointer(window));
        if (self) {
            if (action == GLFW_PRESS)
                self->OnKeyPressed.Broadcast(key);
            if (action == GLFW_RELEASE)
                self->OnKeyReleased.Broadcast(key);
        }
    });

    // Mouse button callback
    glfwSetMouseButtonCallback(m_window, [](GLFWwindow *window, I32 button, I32 action, I32 /*mods*/) {
        auto *self = static_cast<JzGLFWWindowBackend *>(glfwGetWindowUserPointer(window));
        if (self) {
            if (action == GLFW_PRESS)
                self->OnMouseButtonPressed.Broadcast(button);
            if (action == GLFW_RELEASE)
                self->OnMouseButtonReleased.Broadcast(button);
        }
    });

    // Scroll callback
    glfwSetScrollCallback(m_window, [](GLFWwindow *window, F64 xOffset, F64 yOffset) {
        auto *self = static_cast<JzGLFWWindowBackend *>(glfwGetWindowUserPointer(window));
        if (self) {
            self->OnMouseScrolled.Broadcast({static_cast<F32>(xOffset), static_cast<F32>(yOffset)});
        }
    });

    // Window size callback
    glfwSetWindowSizeCallback(m_window, [](GLFWwindow *window, I32 width, I32 height) {
        auto *self = static_cast<JzGLFWWindowBackend *>(glfwGetWindowUserPointer(window));
        if (self) {
            self->m_size = {width, height};
            self->OnWindowResized.Broadcast({width, height});
        }
    });

    // Framebuffer size callback
    glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow *window, I32 width, I32 height) {
        auto *self = static_cast<JzGLFWWindowBackend *>(glfwGetWindowUserPointer(window));
        if (self) {
            self->OnFrameBufferResized.Broadcast({width, height});
        }
    });

    // Cursor position callback
    glfwSetCursorPosCallback(m_window, [](GLFWwindow *window, F64 x, F64 y) {
        auto *self = static_cast<JzGLFWWindowBackend *>(glfwGetWindowUserPointer(window));
        if (self) {
            self->OnCursorMoved.Broadcast({static_cast<I32>(x), static_cast<I32>(y)});
        }
    });

    // Window position callback
    glfwSetWindowPosCallback(m_window, [](GLFWwindow *window, I32 x, I32 y) {
        auto *self = static_cast<JzGLFWWindowBackend *>(glfwGetWindowUserPointer(window));
        if (self) {
            if (!self->m_fullscreen) {
                self->m_position = {x, y};
            }
            self->OnWindowMoved.Broadcast({x, y});
        }
    });

    // Iconify callback
    glfwSetWindowIconifyCallback(m_window, [](GLFWwindow *window, I32 iconified) {
        auto *self = static_cast<JzGLFWWindowBackend *>(glfwGetWindowUserPointer(window));
        if (self) {
            if (iconified == GLFW_TRUE)
                self->OnWindowMinimized.Broadcast();
            if (iconified == GLFW_FALSE)
                self->OnWindowMaximized.Broadcast();
        }
    });

    // Focus callback
    glfwSetWindowFocusCallback(m_window, [](GLFWwindow *window, I32 focused) {
        auto *self = static_cast<JzGLFWWindowBackend *>(glfwGetWindowUserPointer(window));
        if (self) {
            if (focused == GLFW_TRUE)
                self->OnWindowFocusGained.Broadcast();
            if (focused == GLFW_FALSE)
                self->OnWindowFocusLost.Broadcast();
        }
    });

    // Close callback
    glfwSetWindowCloseCallback(m_window, [](GLFWwindow *window) {
        auto *self = static_cast<JzGLFWWindowBackend *>(glfwGetWindowUserPointer(window));
        if (self) {
            self->OnWindowClosed.Broadcast();
        }
    });
}

void JzGLFWWindowBackend::UpdateSizeLimit()
{
    if (m_window) {
        glfwSetWindowSizeLimits(
            m_window,
            m_minimumSize.x,
            m_minimumSize.y,
            m_maximumSize.x,
            m_maximumSize.y);
    }
}

} // namespace JzRE
