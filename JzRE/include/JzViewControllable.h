#pragma once

#include "CommonTypes.h"
#include "JzView.h"

namespace JzRE {
/**
 * @brief All view controllable classes inherit from this class
 */
class JzViewControllable : public JzView {
public:
    /**
     * @brief Constructor
     *
     * @param name
     * @param is_opened
     */
    JzViewControllable(const String &name, Bool is_opened);

    /**
     * @brief Update the view
     *
     * @param deltaTime
     */
    virtual void Update(F32 deltaTime) = 0;
};
} // namespace JzRE