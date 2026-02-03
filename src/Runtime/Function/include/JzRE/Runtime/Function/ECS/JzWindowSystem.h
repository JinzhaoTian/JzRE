/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <memory>

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Core/JzVector.h"
#include "JzRE/Runtime/Function/ECS/JzSystem.h"
#include "JzRE/Runtime/Function/ECS/JzWorld.h"
#include "JzRE/Runtime/Function/ECS/JzWindowComponents.h"
#include "JzRE/Runtime/Function/ECS/JzInputComponents.h"
#include "JzRE/Runtime/Function/Event/JzPlatformEventAdapter.h"
#include "JzRE/Runtime/Platform/RHI/JzDevice.h"
#include "JzRE/Runtime/Platform/Window/JzWindowConfig.h"

namespace JzRE {

// Forward declarations
class JzEventSystem;
class JzIWindowBackend;

/**
 * @brief System that manages the window backend and synchronizes with ECS components.
 *
 * This system delegates all platform-specific window operations to a JzIWindowBackend
 * implementation (default: JzGLFWWindowBackend).
 *
 * Responsibilities:
 * - Manage the window backend lifecycle
 * - Poll window events from the backend
 * - Update JzWindowStateComponent from backend state
 * - Apply component changes to the backend (bidirectional sync)
 * - Process window event queue
 * - Update JzInputStateComponent with raw input from window callbacks
 * - Provide public API for external consumers (Editor, ImGui, etc.)
 *
 * This system runs in the Input phase (first phase) to ensure
 * fresh window/input state for all subsequent systems.
 */
class JzWindowSystem : public JzSystem {
public:
    JzWindowSystem();
    ~JzWindowSystem() override;

    // ==================== ECS System Lifecycle ====================

    void OnInit(JzWorld &world) override;
    void Update(JzWorld &world, F32 delta) override;
    void OnShutdown(JzWorld &world) override;

    JzSystemPhase GetPhase() const override
    {
        return JzSystemPhase::Input;
    }

    // ==================== Backend Management ====================

    /**
     * @brief Set a custom window backend.
     *
     * Must be called before InitializeWindow() if a non-default backend is desired.
     * If not called, InitializeWindow() will create a JzGLFWWindowBackend by default.
     *
     * @param backend The window backend to use.
     */
    void SetBackend(std::unique_ptr<JzIWindowBackend> backend);

    // ==================== Window Creation ====================

    /**
     * @brief Initialize the window with the given configuration.
     *
     * Creates a default JzGLFWWindowBackend if no backend was set via SetBackend().
     * This must be called before using any window operations.
     *
     * @param rhiType The RHI type for setting context hints.
     * @param config Window configuration.
     */
    void InitializeWindow(JzERHIType rhiType, const JzWindowConfig &config);

    /**
     * @brief Create a window entity in the ECS world with the given configuration.
     *
     * @param world The ECS world.
     * @param config Window configuration.
     * @return The created window entity.
     */
    JzEntity CreateWindowEntity(JzWorld &world, const JzWindowConfig &config);

    /**
     * @brief Destroy the window and cleanup resources.
     */
    void ReleaseWindow();

    // ==================== Window Operations ====================

    /**
     * @brief Make the OpenGL context current on the calling thread.
     */
    void MakeCurrentContext() const;

    /**
     * @brief Detach the OpenGL context from the current thread.
     */
    void DetachContext() const;

    /**
     * @brief Poll events from the windowing system.
     */
    void PollWindowEvents() const;

    /**
     * @brief Get the underlying window backend.
     *
     * @return JzIWindowBackend* Backend pointer or nullptr if not initialized.
     */
    JzIWindowBackend *GetBackend() const
    {
        return m_backend.get();
    }

    // ==================== Window Properties ====================

    /**
     * @brief Get the windowing library's window handle (e.g., GLFWwindow* as void*).
     *
     * Used for library-specific integrations such as ImGui backends.
     */
    void *GetPlatformWindowHandle() const;

    /**
     * @brief Get the OS-native window handle (HWND, NSWindow*, X11 Window).
     */
    void *GetNativeWindow() const;

    void   SetTitle(const String &title);
    String GetTitle() const;

    void    SetPosition(JzIVec2 position);
    JzIVec2 GetPosition() const;

    void    SetSize(JzIVec2 size);
    JzIVec2 GetSize() const;

    JzIVec2 GetFramebufferSize() const;
    JzIVec2 GetMonitorSize() const;

    void    SetMinimumSize(JzIVec2 minimumSize);
    JzIVec2 GetMinimumSize() const;

    void    SetMaximumSize(JzIVec2 maximumSize);
    JzIVec2 GetMaximumSize() const;

    // ==================== Window State ====================

    void SetFullscreen(Bool value);
    Bool IsFullscreen() const;

    void SetAlignCentered();

    Bool IsMinimized() const;
    void Minimize() const;
    void Restore() const;

    Bool IsMaximized() const;
    void Maximize() const;

    Bool IsHidden() const;
    Bool IsVisible() const;
    void Hide() const;
    void Show() const;

    Bool IsFocused() const;
    void Focus() const;

    void SetShouldClose(Bool value) const;
    Bool ShouldClose() const;

    // ==================== Input Polling ====================

    /**
     * @brief Poll the current state of a keyboard key.
     * @return true if pressed, false if released.
     */
    Bool GetKeyState(I32 key) const;

    /**
     * @brief Poll the current state of a mouse button.
     * @return true if pressed, false if released.
     */
    Bool GetMouseButtonState(I32 button) const;

    /**
     * @brief Get the current cursor position in window coordinates.
     */
    JzVec2 GetCursorPosition() const;

    // ==================== Entity Management ====================

    JzEntity GetPrimaryWindow() const
    {
        return m_primaryWindow;
    }
    void SetPrimaryWindow(JzEntity window)
    {
        m_primaryWindow = window;
    }

private:
    // ==================== ECS Update Helpers ====================

    void PollEvents(JzWorld &world);
    void UpdateWindowState(JzWorld &world);
    void ApplyComponentChanges(JzWorld &world);
    void ProcessWindowEvents(JzWorld &world);
    void UpdateInputState(JzWorld &world);
    void SwapBuffers(JzWorld &world);
    void UpdateStatistics(JzWorld &world, F32 delta);
    void HandleCloseRequests(JzWorld &world);
    void SyncInputFromBackend(JzWorld &world, JzEntity windowEntity);
    void EmitWindowEvents(JzWorld &world);
    void ProcessPlatformEvents(JzWorld &world);
    void SwapWindowBuffers() const;

private:
    // ==================== Backend ====================

    std::unique_ptr<JzIWindowBackend> m_backend;

    // ==================== Platform Event Adapter ====================

    JzPlatformEventAdapter m_eventAdapter;

    // ==================== ECS State ====================

    JzEntity m_primaryWindow{};

    // Statistics tracking
    F64 m_accumulatedTime{0.0};
    I32 m_frameCount{0};

    // Cached previous window state for event emission (change detection)
    JzIVec2 m_prevSize{0, 0};
    JzIVec2 m_prevPosition{0, 0};
    JzVec2  m_pendingScrollDelta{0.0f, 0.0f};
    Bool    m_prevFocused{false};
    Bool    m_prevMinimized{false};
    Bool    m_prevMaximized{false};
    Bool    m_prevShouldClose{false};
    Bool    m_eventStateInitialized{false};
};

} // namespace JzRE
