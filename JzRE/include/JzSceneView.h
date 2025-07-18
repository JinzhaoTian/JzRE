#pragma once

#include "CommonTypes.h"
#include "JzViewControllable.h"

namespace JzRE {
class JzSceneView : public JzViewControllable {
public:
    JzSceneView();
    ~JzSceneView();

    /**
     * @brief Update the scene view
     *
     * @param deltaTime
     */
    virtual void Update(F32 deltaTime) override;

    /**
     * @brief Render the scene view
     */
    virtual void Render() override;
};
} // namespace JzRE