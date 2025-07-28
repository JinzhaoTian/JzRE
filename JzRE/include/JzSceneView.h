#pragma once

#include "CommonTypes.h"
#include "JzViewControllable.h"

namespace JzRE {
/**
 * @brief Scene View
 */
class JzSceneView : public JzViewControllable {
public:
    /**
     * @brief Constructor
     *
     * @param name
     * @param is_opened
     */
    JzSceneView(const String &name, Bool is_opened);

    /**
     * @brief Update the scene view
     *
     * @param deltaTime
     */
    virtual void Update(F32 deltaTime) override;
};
} // namespace JzRE