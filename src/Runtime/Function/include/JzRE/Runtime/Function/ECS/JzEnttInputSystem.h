/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Core/JzVector.h"
#include "JzRE/Runtime/Function/ECS/JzEnttSystem.h"
#include "JzRE/Runtime/Function/ECS/JzEnttWorld.h"

namespace JzRE {

/**
 * @brief System that processes raw input and updates input components.
 *
 * This system acts as a bridge between the low-level JzInputManager
 * (Platform Layer) and the high-level ECS input components (Function Layer).
 *
 * Responsibilities:
 * - Read raw input state from JzInputManager
 * - Update JzEnttMouseInputComponent with mouse state and deltas
 * - Update JzEnttKeyboardInputComponent with keyboard state
 * - Update camera-specific input components (JzEnttCameraInputComponent)
 * - Process input mappings and dead zones (future enhancement)
 *
 * This system runs in the Input phase (first Logic phase) to ensure
 * all other systems have fresh input data available.
 */
class JzEnttInputSystem : public JzEnttSystem {
public:
    JzEnttInputSystem() = default;

    void OnInit(JzEnttWorld &world) override;
    void Update(JzEnttWorld &world, F32 delta) override;

    /**
     * @brief Input system runs in the Input phase (first logic phase).
     */
    JzSystemPhase GetPhase() const override
    {
        return JzSystemPhase::Input;
    }

private:
    /**
     * @brief Update mouse input components from raw input manager.
     */
    void UpdateMouseInput(JzEnttWorld &world);

    /**
     * @brief Update keyboard input components from raw input manager.
     */
    void UpdateKeyboardInput(JzEnttWorld &world);

    /**
     * @brief Update camera-specific input components.
     *
     * Processes raw mouse/keyboard input and translates it into
     * camera control signals (orbit, pan, zoom).
     */
    void UpdateCameraInput(JzEnttWorld &world);

private:
    // Cached previous mouse position for delta calculation
    JzVec2 m_previousMousePosition{0.0f, 0.0f};
    Bool   m_firstFrame{true};
};

} // namespace JzRE
