/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <bitset>
#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Function/ECS/JzSystem.h"
#include "JzRE/Runtime/Function/ECS/JzWorld.h"
#include "JzRE/Runtime/Function/ECS/JzInputComponents.h"

namespace JzRE {

// Forward declarations
class JzEventSystem;

/**
 * @brief ECS system that processes raw input and updates input components.
 *
 * Reads from JzInputStateComponent (populated by JzWindowSystem via GLFW
 * callbacks) and updates higher-level input components for consumption by
 * other systems.
 *
 * Responsibilities:
 * - Sync JzMouseInputComponent/JzKeyboardInputComponent from JzInputStateComponent
 * - Process camera-specific input (JzCameraInputComponent, JzCameraInputStateComponent)
 * - Update JzInputActionComponent action values
 * - Emit typed ECS events (JzKeyEvent, JzMouseButtonEvent, etc.)
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
     * @brief Sync higher-level input components from JzInputStateComponent.
     */
    void SyncLegacyComponentsFromInputState(JzWorld &world);

    /**
     * @brief Get the primary input state component (from primary window entity).
     */
    JzInputStateComponent *GetPrimaryInputState(JzWorld &world);

    /**
     * @brief Calculate binding value from input state.
     */
    F32 GetBindingValue(const JzInputStateComponent           &input,
                        const JzInputActionComponent::Binding &binding);

    /**
     * @brief Apply deadzone to axis value.
     */
    F32 ApplyDeadzone(F32 value, F32 deadzone);

    /**
     * @brief Emit keyboard key events through the event dispatcher.
     *
     * Diffs current keysPressed vs previous frame to emit
     * JzKeyEvent with Pressed/Released action.
     */
    void EmitKeyboardEvents(JzWorld &world);

    /**
     * @brief Emit mouse events through the event dispatcher.
     *
     * Emits JzMouseButtonEvent on button state changes,
     * JzMouseMoveEvent on movement, JzMouseScrollEvent on scroll.
     */
    void EmitMouseEvents(JzWorld &world);

    /**
     * @brief Emit high-level action events through the event dispatcher.
     *
     * Emits JzInputActionTriggeredEvent / JzInputActionReleasedEvent
     * when actions change state.
     */
    void EmitActionEvents(JzWorld &world);

private:
    // Cached primary window entity for quick access
    JzEntity m_primaryWindowEntity{};

    // Cached previous input state for event emission (change detection)
    std::bitset<512> m_prevKeysPressed;
    std::bitset<8>   m_prevButtonsPressed;
};

} // namespace JzRE
