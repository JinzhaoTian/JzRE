#pragma once

#include "CommonTypes.h"
#include "JzCamera.h"
#include "JzCameraController.h"
#include "JzView.h"

namespace JzRE {
/**
 * @brief Controllable View
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
    virtual void Update(F32 deltaTime) override;

    /**
     * @brief Initialize the frame
     */
    virtual void InitFrame() override;

    /**
     * @brief Reset the camera transform
     */
    virtual void ResetCameraTransform();

    /**
     * @brief Get the camera controller
     *
     * @return The camera controller
     */
    JzCameraController &GetCameraController();

    /**
     * @brief Get the camera
     *
     * @return The camera
     */
    virtual JzCamera *GetCamera() override;

    /**
     * @brief Reset the clear color
     */
    void ResetClearColor();

protected:
    JzCamera           m_camera;
    JzCameraController m_cameraController;
};
} // namespace JzRE