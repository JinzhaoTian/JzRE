/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Core/JzMatrix.h"
#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Core/JzVector.h"
#include "JzRE/Runtime/Function/ECS/JzEnttRenderComponents.h"
#include "JzRE/Runtime/Function/ECS/JzEnttSystem.h"
#include "JzRE/Runtime/Function/ECS/JzEnttWorld.h"

namespace JzRE {

/**
 * @brief System that updates camera view and projection matrices.
 *
 * This system handles:
 * - Computing view and projection matrices for all cameras
 * - Processing orbit camera controller input
 * - Caching main camera data for use by other systems
 */
class JzEnttCameraSystem : public JzEnttSystem {
public:
    JzEnttCameraSystem() = default;

    void OnInit(JzEnttWorld &world) override;
    void Update(JzEnttWorld &world, F32 delta) override;

    /**
     * @brief Get the main camera's view matrix.
     */
    const JzMat4 &GetViewMatrix() const
    {
        return m_viewMatrix;
    }

    /**
     * @brief Get the main camera's projection matrix.
     */
    const JzMat4 &GetProjectionMatrix() const
    {
        return m_projectionMatrix;
    }

    /**
     * @brief Get the main camera's position.
     */
    const JzVec3 &GetCameraPosition() const
    {
        return m_cameraPosition;
    }

    /**
     * @brief Get the main camera's clear color.
     */
    const JzVec3 &GetClearColor() const
    {
        return m_clearColor;
    }

    /**
     * @brief Set the aspect ratio for all cameras.
     *
     * @param aspect The width/height aspect ratio.
     */
    void SetAspectRatio(F32 aspect)
    {
        m_aspectRatio = aspect;
    }

private:
    /**
     * @brief Update camera matrices from its parameters.
     */
    void UpdateCameraMatrices(JzEnttCameraComponent &camera);

    /**
     * @brief Handle orbit controller input and update camera position/rotation.
     */
    void HandleOrbitController(JzEnttCameraComponent          &camera,
                               JzEnttOrbitControllerComponent &orbit);

    /**
     * @brief Handle orbit rotation from mouse movement.
     */
    void HandleOrbitRotation(JzEnttOrbitControllerComponent &orbit, F32 deltaX, F32 deltaY);

    /**
     * @brief Handle camera panning from mouse movement.
     */
    void HandlePanning(JzEnttOrbitControllerComponent &orbit, F32 deltaX, F32 deltaY);

    /**
     * @brief Handle zoom from scroll wheel.
     */
    void HandleZoom(JzEnttOrbitControllerComponent &orbit, F32 scrollY);

    /**
     * @brief Update camera position and rotation from orbit parameters.
     */
    void UpdateCameraFromOrbit(JzEnttCameraComponent          &camera,
                               JzEnttOrbitControllerComponent &orbit);

    // Cached main camera data
    JzMat4 m_viewMatrix       = JzMat4x4::Identity();
    JzMat4 m_projectionMatrix = JzMat4x4::Identity();
    JzVec3 m_cameraPosition{0.0f, 0.0f, 10.0f};
    JzVec3 m_clearColor{0.1f, 0.1f, 0.1f};
    F32    m_aspectRatio = 16.0f / 9.0f;
};

} // namespace JzRE
