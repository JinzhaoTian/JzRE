/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Platform/Window/IWindowBackend.h"

struct GLFWwindow;

namespace JzRE {

/**
 * @brief GLFW implementation of the window backend.
 *
 * Encapsulates all GLFW-specific window management:
 * window creation, callback setup, property access, input polling,
 * and native handle retrieval.
 */
class JzGLFWWindowBackend : public IWindowBackend {
public:
    JzGLFWWindowBackend() = default;
    ~JzGLFWWindowBackend() override;

    // ==================== Lifecycle ====================

    void Initialize(JzERHIType rhiType, const JzWindowConfig &config) override;
    void Shutdown() override;
    Bool IsValid() const override;

    // ==================== Context Management ====================

    void MakeContextCurrent() override;
    void DetachContext() override;
    void SwapBuffers() override;

    // ==================== Event Polling ====================

    void PollEvents() override;

    // ==================== Window Properties ====================

    void   SetTitle(const String &title) override;
    String GetTitle() const override;

    void    SetPosition(JzIVec2 position) override;
    JzIVec2 GetPosition() const override;

    void    SetSize(JzIVec2 size) override;
    JzIVec2 GetSize() const override;

    JzIVec2 GetFramebufferSize() const override;
    JzIVec2 GetMonitorSize() const override;

    void    SetMinimumSize(JzIVec2 size) override;
    JzIVec2 GetMinimumSize() const override;

    void    SetMaximumSize(JzIVec2 size) override;
    JzIVec2 GetMaximumSize() const override;

    // ==================== Window State ====================

    void SetFullscreen(Bool value) override;
    Bool IsFullscreen() const override;

    void SetAlignCentered() override;

    Bool IsMinimized() const override;
    void Minimize() override;
    void Restore() override;

    Bool IsMaximized() const override;
    void Maximize() override;

    Bool IsHidden() const override;
    Bool IsVisible() const override;
    void Hide() override;
    void Show() override;

    Bool IsFocused() const override;
    void Focus() override;

    void SetShouldClose(Bool value) override;
    Bool ShouldClose() const override;

    // ==================== Input Polling ====================

    Bool   GetKeyState(I32 key) const override;
    Bool   GetMouseButtonState(I32 button) const override;
    JzVec2 GetCursorPosition() const override;

    // ==================== Platform Handles ====================

    void *GetPlatformWindowHandle() const override;
    void *GetNativeWindowHandle() const override;

private:
    void CreateGLFWWindow(const JzWindowConfig &config);
    void SetupCallbacks();
    void UpdateSizeLimit();

private:
    GLFWwindow *m_window{nullptr};
    JzERHIType  m_rhiType{JzERHIType::Unknown};

    // Cached state
    String  m_title;
    JzIVec2 m_size{0, 0};
    JzIVec2 m_position{0, 0};
    JzIVec2 m_minimumSize{-1, -1};
    JzIVec2 m_maximumSize{-1, -1};
    JzIVec2 m_windowedSize{0, 0};
    JzIVec2 m_windowedPos{0, 0};
    Bool    m_fullscreen{false};
    I32     m_refreshRate{-1};
};

} // namespace JzRE
