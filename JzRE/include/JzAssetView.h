#pragma once

#include "CommonTypes.h"
#include "JzViewControllable.h"

namespace JzRE {
class JzAssetView : public JzViewControllable {
public:
    JzAssetView();
    ~JzAssetView();

    /**
     * @brief Update the asset view
     *
     * @param deltaTime
     */
    virtual void Update(F32 deltaTime) override;
};
} // namespace JzRE