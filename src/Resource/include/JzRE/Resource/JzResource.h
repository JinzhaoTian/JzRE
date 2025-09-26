/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Core/JzRETypes.h"

namespace JzRE {

/**
 * @brief Enums of resource state
 */
enum class JzEResourceState {
    Unloaded,
    Loading,
    Loaded,
    Error
};

/**
 * @brief Interface for JzRE resource
 */
class JzResource {
public:
    /**
     * @brief Destructor
     */
    virtual ~JzResource() = default;

    /**
     * @brief Get the State object
     *
     * @return JzEResourceState
     */
    JzEResourceState GetState() const
    {
        return m_state;
    }

    /**
     * @brief Get the Name object
     *
     * @return const String&
     */
    const String &GetName() const
    {
        return m_name;
    }

    /**
     * @brief Load a resource method
     *
     * @return Bool
     */
    virtual Bool Load() = 0;

    /**
     * @brief Unload a resource method
     *
     */
    virtual void Unload() = 0;

protected:
    JzEResourceState m_state = JzEResourceState::Unloaded;
    String           m_name;
};

} // namespace JzRE