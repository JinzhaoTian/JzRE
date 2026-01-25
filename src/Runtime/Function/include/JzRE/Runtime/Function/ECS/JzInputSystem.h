/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Core/JzVector.h"
#include "JzRE/Runtime/Function/ECS/JzSystem.h"
#include "JzRE/Runtime/Function/ECS/JzWorld.h"
#include "JzRE/Runtime/Function/ECS/JzInputComponents.h"

namespace JzRE {

/**
 * @brief System that processes raw input and updates input components.
 *
 * This system processes input state and updates ECS input components.
 * It supports two modes of operation:
 *
 * 1. ECS Mode (Preferred): Reads from JzInputStateComponent (populated
 *    by JzWindowSystem) and updates legacy components for compatibility.
 *
 * 2. Legacy Mode: Falls back to JzInputManager if JzInputStateComponent
 *    is not available on any window entity.
 *
 * Responsibilities:
 * - Sync JzInputStateComponent from window backend (via JzWindowSystem)
 * - Update legacy JzMouseInputComponent and JzKeyboardInputComponent
 * - Process camera-specific input (JzCameraInputComponent, JzCameraInputStateComponent)
 * - Update JzInputActionComponent action values
 * - Clear per-frame input state at end of frame
 *
 * This system runs in the Input phase (first Logic phase) to ensure
 * all other systems have fresh input data available.
 */
class JzInputSystem : public JzSystem {
public:
    JzInputSystem() = default;

    void OnInit(JzWorld &world) override;
    void Update(JzWorld &world, F32 delta) override;
    void OnShutdown(JzWorld &world) override;

    /**
     * @brief Input system runs in the Input phase (first logic phase).
     */
    JzSystemPhase GetPhase() const override
    {
        return JzSystemPhase::Input;
    }

    /**
     * @brief Clear per-frame input state. Call at end of frame.
     */
    void ClearFrameState(JzWorld &world);

private:
    /**
     * @brief Update mouse input components from JzInputStateComponent or legacy manager.
     */
    void UpdateMouseInput(JzWorld &world);

    /**
     * @brief Update keyboard input components from JzInputStateComponent or legacy manager.
     */
    void UpdateKeyboardInput(JzWorld &world);

    /**
     * @brief Update camera-specific input components.
     *
     * Processes raw mouse/keyboard input and translates it into
     * camera control signals (orbit, pan, zoom).
     */
    void UpdateCameraInput(JzWorld &world);

    /**
     * @brief Update input actions from bindings.
     */
    void UpdateInputActions(JzWorld &world, F32 delta);

    /**
     * @brief Sync legacy components from JzInputStateComponent.
     */
    void SyncLegacyComponentsFromInputState(JzWorld &world);

    /**
     * @brief Get the primary input state component (from primary window entity).
     */
    JzInputStateComponent* GetPrimaryInputState(JzWorld &world);

    /**
     * @brief Calculate binding value from input state.
     */
    F32 GetBindingValue(const JzInputStateComponent &input,
                        const JzInputActionComponent::Binding &binding);

    /**
     * @brief Apply deadzone to axis value.
     */
    F32 ApplyDeadzone(F32 value, F32 deadzone);

private:
    // Cached previous mouse position for delta calculation (legacy mode)
    JzVec2 m_previousMousePosition{0.0f, 0.0f};
    Bool   m_firstFrame{true};

    // Cached primary window entity for quick access
    JzEntity m_primaryWindowEntity{};
};

} // namespace JzRE
