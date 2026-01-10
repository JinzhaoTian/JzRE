/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Core/JzRETypes.h"

namespace JzRE {

// Forward declaration
class JzEnttWorld;

/**
 * @brief Abstract base class for all EnTT-based systems.
 *
 * Systems in the EnTT ECS framework process entities with specific component
 * combinations. Each system should override the Update method to implement
 * its logic.
 */
class JzEnttSystem {
public:
    /**
     * @brief Default constructor.
     */
    JzEnttSystem() = default;

    /**
     * @brief Virtual destructor.
     */
    virtual ~JzEnttSystem() = default;

    // Systems are non-copyable but movable
    JzEnttSystem(const JzEnttSystem &)            = delete;
    JzEnttSystem &operator=(const JzEnttSystem &) = delete;
    JzEnttSystem(JzEnttSystem &&)                 = default;
    JzEnttSystem &operator=(JzEnttSystem &&)      = default;

    /**
     * @brief The update method called every frame.
     *
     * @param world The EnTT world containing entities and components.
     * @param delta The delta time since the last frame.
     */
    virtual void Update(JzEnttWorld &world, F32 delta) = 0;

    /**
     * @brief Optional initialization method called when the system is registered.
     *
     * @param world The EnTT world containing entities and components.
     */
    virtual void OnInit(JzEnttWorld &world)
    {
        // Default implementation does nothing
    }

    /**
     * @brief Optional cleanup method called when the system is destroyed.
     *
     * @param world The EnTT world containing entities and components.
     */
    virtual void OnShutdown(JzEnttWorld &world)
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

private:
    Bool m_enabled = true; ///< Whether the system is enabled.
};

} // namespace JzRE
