/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Function/ECS/JzCameraComponents.h"
#include "JzRE/Runtime/Function/ECS/JzSystem.h"
#include "JzRE/Runtime/Function/ECS/JzWorld.h"

namespace JzRE {

/**
 * @brief System that updates camera view and projection matrices.
 *
 * This system handles:
 * - Computing view and projection matrices for all cameras
 * - Processing orbit camera controller input
 * - Caching main camera data for use by other systems
 */
class JzCameraSystem : public JzSystem {
public:
    JzCameraSystem() = default;

    void OnInit(JzWorld &world) override;
    void Update(JzWorld &world, F32 delta) override;

    /**
     * @brief Camera system runs in PreRender phase.
     */
    JzSystemPhase GetPhase() const override
    {
        return JzSystemPhase::PreRender;
    }

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



private:
    /**
     * @brief Update camera matrices from its parameters.
     */
    void UpdateCameraMatrices(JzCameraComponent &camera);

    /**
     * @brief Handle orbit controller input and update camera position/rotation.
     *
     * @param camera Camera component to update
     * @param orbit Orbit controller component with orbit parameters
     * @param input Camera input component with processed input state
     */
    void HandleOrbitController(JzCameraComponent            &camera,
                               JzOrbitControllerComponent   &orbit,
                               const JzCameraInputComponent &input);

    /**
     * @brief Handle orbit rotation from mouse movement.
     *
     * @param orbit Orbit controller component
     * @param deltaX Horizontal mouse delta
     * @param deltaY Vertical mouse delta
     */
    void HandleOrbitRotation(JzOrbitControllerComponent &orbit, F32 deltaX, F32 deltaY);

    /**
     * @brief Handle camera panning from mouse movement.
     *
     * @param orbit Orbit controller component
     * @param deltaX Horizontal mouse delta
     * @param deltaY Vertical mouse delta
     */
    void HandlePanning(JzOrbitControllerComponent &orbit, F32 deltaX, F32 deltaY);

    /**
     * @brief Handle zoom from scroll wheel.
     *
     * @param orbit Orbit controller component
     * @param scrollY Scroll wheel delta
     */
    void HandleZoom(JzOrbitControllerComponent &orbit, F32 scrollY);

    /**
     * @brief Update camera position and rotation from orbit parameters.
     */
    void UpdateCameraFromOrbit(JzCameraComponent          &camera,
                               JzOrbitControllerComponent &orbit);

private:
    // Cached main camera data
    JzMat4 m_viewMatrix       = JzMat4x4::Identity();
    JzMat4 m_projectionMatrix = JzMat4x4::Identity();
    JzVec3 m_cameraPosition{0.0f, 0.0f, 10.0f};
    JzVec3 m_clearColor{0.1f, 0.1f, 0.1f};
};

} // namespace JzRE
