#pragma once

#include "CommonTypes.h"
#include "JzViewControllable.h"

namespace JzRE {
/**
 * @brief Asset View
 */
class JzAssetView : public JzViewControllable {
public:
    /**
     * @brief Constructor
     *
     * @param name
     * @param is_opened
     */
    JzAssetView(const String &name, Bool is_opened);

    /**
     * @brief Update the asset view
     *
     * @param deltaTime
     */
    virtual void Update(F32 deltaTime) override;
};
} // namespace JzRE