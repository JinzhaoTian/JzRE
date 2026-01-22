/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Core/JzRETypes.h"

namespace JzRE {

// Forward declaration
class JzWorld;

/**
 * @brief System execution phase for separating logic and rendering updates.
 *
 * Systems are categorized into phases to enable proper synchronization.
 * Phases execute in the following order:
 *
 * === LOGIC PHASES (can run parallel with GPU work) ===
 * 1. Input        - Input processing, event handling
 * 2. Physics      - Physics simulation, collision detection
 * 3. Animation    - Skeletal animation, blend trees
 * 4. Logic        - General game logic, AI, scripts
 *
 * === PRE-RENDER PHASES (after sync point) ===
 * 5. PreRender    - Camera matrices, light collection
 * 6. Culling      - Frustum culling, occlusion culling, LOD selection
 *
 * === RENDER PHASES (GPU rendering) ===
 * 7. RenderPrep   - Render data extraction, batch building, instance data
 * 8. Render       - Actual GPU draw calls
 */
enum class JzSystemPhase : U8 {
    // Logic phases (can run parallel with GPU)
    Input,      ///< Input processing phase
    Physics,    ///< Physics simulation phase
    Animation,  ///< Animation update phase
    Logic,      ///< General game logic phase

    // Pre-render phases (after sync point)
    PreRender,  ///< Camera/light preparation phase
    Culling,    ///< Culling and LOD selection phase

    // Render phases
    RenderPrep, ///< Render data preparation (batching, instancing)
    Render      ///< Actual GPU rendering phase
};

/**
 * @brief Check if a phase is a logic phase (can run parallel with GPU).
 */
inline Bool IsLogicPhase(JzSystemPhase phase)
{
    return phase <= JzSystemPhase::Logic;
}

/**
 * @brief Check if a phase is a pre-render phase.
 */
inline Bool IsPreRenderPhase(JzSystemPhase phase)
{
    return phase == JzSystemPhase::PreRender || phase == JzSystemPhase::Culling;
}

/**
 * @brief Check if a phase is a render phase.
 */
inline Bool IsRenderPhase(JzSystemPhase phase)
{
    return phase >= JzSystemPhase::RenderPrep;
}

/**
 * @brief Abstract base class for all EnTT-based systems.
 *
 * Systems in the EnTT ECS framework process entities with specific component
 * combinations. Each system should override the Update method to implement
 * its logic.
 */
class JzSystem {
public:
    /**
     * @brief Default constructor.
     */
    JzSystem() = default;

    /**
     * @brief Virtual destructor.
     */
    virtual ~JzSystem() = default;

    // Systems are non-copyable but movable
    JzSystem(const JzSystem &)            = delete;
    JzSystem &operator=(const JzSystem &) = delete;
    JzSystem(JzSystem &&)                 = default;
    JzSystem &operator=(JzSystem &&)      = default;

    /**
     * @brief The update method called every frame.
     *
     * @param world The EnTT world containing entities and components.
     * @param delta The delta time since the last frame.
     */
    virtual void Update(JzWorld &world, F32 delta) = 0;

    /**
     * @brief Optional initialization method called when the system is registered.
     *
     * @param world The EnTT world containing entities and components.
     */
    virtual void OnInit(JzWorld &world)
    {
        // Default implementation does nothing
    }

    /**
     * @brief Optional cleanup method called when the system is destroyed.
     *
     * @param world The EnTT world containing entities and components.
     */
    virtual void OnShutdown(JzWorld &world)
    {
        // Default implementation does nothing
    }

    /**
     * @brief Checks if the system is enabled.
     *
     * @return True if the system is enabled, false otherwise.
     */
    Bool IsEnabled() const
    {
        return m_enabled;
    }

    /**
     * @brief Enables or disables the system.
     *
     * @param enabled True to enable, false to disable.
     */
    void SetEnabled(Bool enabled)
    {
        m_enabled = enabled;
    }

    /**
     * @brief Gets the execution phase of this system.
     *
     * Override this method to specify which phase the system runs in.
     * Default is Logic phase.
     *
     * @return The system's execution phase.
     */
    virtual JzSystemPhase GetPhase() const
    {
        return JzSystemPhase::Logic;
    }

private:
    Bool m_enabled = true; ///< Whether the system is enabled.
};

} // namespace JzRE
