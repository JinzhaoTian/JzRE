#pragma once

#include "CommonTypes.h"

namespace JzRE {
/**
 * @brief View controllable
 */
class JzViewControllable {
public:
    /**
     * @brief Update the view
     *
     * @param deltaTime
     */
    virtual void Update(F32 deltaTime) = 0;

    /**
     * @brief Render the view
     */
    virtual void Render() = 0;
};
} // namespace JzRE