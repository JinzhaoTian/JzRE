/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Editor/JzObject.h"

namespace JzRE {

class JzActor;

/**
 * @brief Base class for all actor components in JzRE
 */
class JzActorComponent : public JzObject {
    friend class JzActor;

public:
    /**
     * @brief Gets the owner of this component.
     *
     * @return A pointer to the owning actor.
     */
    JzActor *GetOwner() const
    {
        return m_owner;
    }

private:
    /**
     * @brief Sets the owner of this component.
     *
     * @param owner The actor that owns this component.
     */
    void SetOwner(JzActor *owner)
    {
        m_owner = owner;
    }

private:
    JzActor *m_owner = nullptr;
};

} // namespace JzRE
