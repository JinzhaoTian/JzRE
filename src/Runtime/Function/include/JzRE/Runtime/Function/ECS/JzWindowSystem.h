/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <unordered_map>
#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif
#include <GLFW/glfw3.h>

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Core/JzDelegate.h"
#include "JzRE/Runtime/Core/JzVector.h"
#include "JzRE/Runtime/Function/ECS/JzSystem.h"
#include "JzRE/Runtime/Function/ECS/JzWorld.h"
#include "JzRE/Runtime/Function/ECS/JzWindowComponents.h"
#include "JzRE/Runtime/Function/ECS/JzInputComponents.h"
#include "JzRE/Runtime/Platform/JzDevice.h"

namespace JzRE {

// Forward declarations
class JzEventDispatcherSystem;

/**
 * @brief System that directly manages the GLFW window and synchronizes with ECS components.
 *
 * This system owns the GLFWwindow and handles all platform-specific window operations.
 * It replaces the former JzWindow class by integrating window management directly
 * into the ECS architecture.
 *
 * Responsibilities:
 * - Create and destroy the GLFW window
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
    JzWindowSystem() = default;
    ~JzWindowSystem() override;

    // ==================== ECS System Lifecycle ====================

    void OnInit(JzWorld &world) override;
    void Update(JzWorld &world, F32 delta) override;
    void OnShutdown(JzWorld &world) override;

    JzSystemPhase GetPhase() const override
    {
        return JzSystemPhase::Input;
    }

    // ==================== Window Creation ====================

    /**
     * @brief Create the GLFW window with the given configuration.
     *
     * This must be called before using any window operations.
     * Can be called independently of the ECS lifecycle (for standalone use).
     *
     * @param rhiType The RHI type for setting GLFW context hints.
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
     * @brief Destroy the GLFW window and cleanup resources.
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
     * @brief Swap the front and back buffers.
     */
    void SwapWindowBuffers() const;

    // ==================== Window Properties ====================

    GLFWwindow *GetGLFWWindow() const;
    void       *GetNativeWindow() const;

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

    // ==================== Entity Management ====================

    JzEntity GetPrimaryWindow() const
    {
        return m_primaryWindow;
    }
    void SetPrimaryWindow(JzEntity window)
    {
        m_primaryWindow = window;
    }

    // ==================== Input Events (Delegate) ====================
    // These delegates allow external systems (e.g., JzInputManager) to
    // subscribe to raw input events from the window backend.

    JzDelegate<I32>     KeyboardButtonPressedEvent;
    JzDelegate<I32>     KeyboardButtonReleasedEvent;
    JzDelegate<I32>     MouseButtonPressedEvent;
    JzDelegate<I32>     MouseButtonReleasedEvent;
    JzDelegate<JzIVec2> MouseMovedEvent;
    JzDelegate<JzVec2>  MouseScrolledEvent;

    // ==================== Window Events (Delegate) ====================

    JzDelegate<JzIVec2> WindowResizedEvent;
    JzDelegate<JzIVec2> WindowFrameBufferResizedEvent;
    JzDelegate<JzIVec2> WindowMoveEvent;
    JzDelegate<JzIVec2> WindowCursorMoveEvent;
    JzDelegate<>        WindowMinimizedEvent;
    JzDelegate<>        WindowMaximizedEvent;
    JzDelegate<>        WindowFocusGainEvent;
    JzDelegate<>        WindowFocusLostEvent;
    JzDelegate<>        WindowClosedEvent;

private:
    // ==================== GLFW Window Creation ====================

    void CreateGlfwWindow(const JzWindowConfig &config);
    void SetupCallbacks();
    void UpdateSizeLimit() const;

    void OnResize(JzIVec2 size);
    void OnMove(JzIVec2 position);

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

    // ==================== Static Callback Helpers ====================

    static JzWindowSystem                                    *FindInstance(GLFWwindow *glfwWindow);
    static std::unordered_map<GLFWwindow *, JzWindowSystem *> s_windowMap;

private:
    // ==================== GLFW State ====================

    GLFWwindow *m_glfwWindow{nullptr};
    JzERHIType  m_rhiType{JzERHIType::Unknown};
    String      m_title;
    JzIVec2     m_size{0, 0};
    JzIVec2     m_position{0, 0};
    JzIVec2     m_minimumSize{-1, -1};
    JzIVec2     m_maximumSize{-1, -1};
    JzIVec2     m_windowedSize{0, 0};
    JzIVec2     m_windowedPos{0, 0};
    Bool        m_fullscreen{false};
    I32         m_refreshRate{-1};

    // ==================== ECS State ====================

    JzEntity m_primaryWindow{};

    // Statistics tracking
    F64 m_accumulatedTime{0.0};
    I32 m_frameCount{0};

    // Cached previous mouse position for delta calculation
    JzVec2 m_previousMousePosition{0.0f, 0.0f};
    Bool   m_firstFrame{true};

    // Cached previous window state for event emission (change detection)
    JzIVec2 m_prevSize{0, 0};
    JzIVec2 m_prevPosition{0, 0};
    Bool    m_prevFocused{false};
    Bool    m_prevMinimized{false};
    Bool    m_prevMaximized{false};
    Bool    m_prevShouldClose{false};
    Bool    m_eventStateInitialized{false};
};

} // namespace JzRE
