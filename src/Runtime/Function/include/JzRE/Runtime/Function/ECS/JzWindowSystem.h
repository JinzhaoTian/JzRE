/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Function/ECS/JzSystem.h"
#include "JzRE/Runtime/Function/ECS/JzWorld.h"
#include "JzRE/Runtime/Function/ECS/JzWindowComponents.h"
#include "JzRE/Runtime/Function/ECS/JzInputComponents.h"

namespace JzRE {

// Forward declarations
class JzWindow;

/**
 * @brief System that manages windows and synchronizes with the window backend.
 *
 * This system acts as a bridge between the ECS window components and
 * the platform-specific window backend (JzWindow/GLFW).
 *
 * Responsibilities:
 * - Poll window events from the backend
 * - Update JzWindowStateComponent from backend state
 * - Apply component changes to the backend (bidirectional sync)
 * - Process window event queue
 * - Update JzInputStateComponent with raw input from window callbacks
 *
 * This system runs in the Input phase (first phase) to ensure
 * fresh window/input state for all subsequent systems.
 */
class JzWindowSystem : public JzSystem {
public:
    JzWindowSystem() = default;
    ~JzWindowSystem() override = default;

    /**
     * @brief Initialize the window system.
     *
     * Creates the primary window entity if not already present.
     */
    void OnInit(JzWorld &world) override;

    /**
     * @brief Update window and input state.
     */
    void Update(JzWorld &world, F32 delta) override;

    /**
     * @brief Cleanup window resources.
     */
    void OnShutdown(JzWorld &world) override;

    /**
     * @brief Window system runs in the Input phase.
     */
    JzSystemPhase GetPhase() const override {
        return JzSystemPhase::Input;
    }

    /**
     * @brief Get the primary window entity.
     */
    JzEntity GetPrimaryWindow() const { return m_primaryWindow; }

    /**
     * @brief Create a window entity with the given configuration.
     *
     * @param world The ECS world.
     * @param config Window configuration.
     * @return The created window entity.
     */
    JzEntity CreateWindowEntity(JzWorld &world, const JzWindowConfig &config);

    /**
     * @brief Set the primary window entity.
     */
    void SetPrimaryWindow(JzEntity window) { m_primaryWindow = window; }

private:
    /**
     * @brief Poll events from the backend and update components.
     */
    void PollEvents(JzWorld &world);

    /**
     * @brief Update window state component from backend.
     */
    void UpdateWindowState(JzWorld &world);

    /**
     * @brief Apply component changes to backend.
     */
    void ApplyComponentChanges(JzWorld &world);

    /**
     * @brief Process window event queue.
     */
    void ProcessWindowEvents(JzWorld &world);

    /**
     * @brief Update input state from window callbacks.
     */
    void UpdateInputState(JzWorld &world);

    /**
     * @brief Swap buffers for visible windows.
     */
    void SwapBuffers(JzWorld &world);

    /**
     * @brief Update window statistics (FPS, frame time).
     */
    void UpdateStatistics(JzWorld &world, F32 delta);

    /**
     * @brief Handle window close requests.
     */
    void HandleCloseRequests(JzWorld &world);

    /**
     * @brief Synchronize input state component from backend.
     */
    void SyncInputFromBackend(JzWorld &world, JzEntity windowEntity);

private:
    JzEntity m_primaryWindow{};

    // Statistics tracking
    F64 m_accumulatedTime{0.0};
    I32 m_frameCount{0};

    // Cached previous mouse position for delta calculation
    JzVec2 m_previousMousePosition{0.0f, 0.0f};
    Bool   m_firstFrame{true};
};

} // namespace JzRE
