#pragma once

#include "CommonTypes.h"
#include "JzEMode.h"
#include "JzEntity.h"
#include "JzMatrix.h"
#include "JzVector.h"

namespace JzRE {
/**
 * @brief Camera
 */
class JzCamera : public JzEntity {
public:
    /**
     * @brief Constructor
     */
    JzCamera();

    /**
     * @brief Set the camera position
     *
     * @param position
     */
    void SetPosition(const JzVec3 &position);

    /**
     * @brief Get the camera position
     *
     * @return The camera position
     */
    const JzVec3 &GetPosition() const;

    /**
     * @brief Set the camera rotation
     *
     * @param rotation
     */
    void SetRotation(const JzVec4 &rotation);

    /**
     * @brief Get the camera rotation
     *
     * @return The camera rotation
     */
    const JzVec4 &GetRotation() const;

    /**
     * @brief Set the camera size
     *
     * @param value The camera size
     */
    void SetSize(F32 value);

    /**
     * @brief Get the camera size
     *
     * @return The camera size
     */
    F32 GetSize() const;

    /**
     * @brief Set the field of view
     *
     * @param value The field of view
     */
    void SetFov(F32 value);

    /**
     * @brief Get the field of view
     *
     * @return The field of view
     */
    F32 GetFov() const;

    /**
     * @brief Set the near plane
     *
     * @param value The near plane
     */
    void SetNear(F32 value);

    /**
     * @brief Get the near plane
     *
     * @return The near plane
     */
    F32 GetNear() const;

    /**
     * @brief Set the far plane
     *
     * @param value The far plane
     */
    void SetFar(F32 value);

    /**
     * @brief Get the far plane
     *
     * @return The far plane
     */
    F32 GetFar() const;

    /**
     * @brief Set the clear color
     *
     * @param clearColor The clear color
     */
    void SetClearColor(const JzVec3 &clearColor);

    /**
     * @brief Get the clear color
     *
     * @return The clear color
     */
    const JzVec3 &GetClearColor() const;

    /**
     * @brief Get the projection matrix
     *
     * @return The projection matrix
     */
    const JzMat4 &GetProjectionMatrix() const;

    /**
     * @brief Get the view matrix
     *
     * @return The view matrix
     */
    const JzMat4 &GetViewMatrix() const;

    /**
     * @brief Set the projection mode
     *
     * @param projectionMode
     */
    void SetProjectionMode(JzEProjectionMode projectionMode);

    /**
     * @brief Get the projection mode
     *
     * @return The projection mode
     */
    JzEProjectionMode GetProjectionMode() const;

private:
    JzVec3            m_position;
    JzVec4            m_rotation; // TODO Quaternion
    JzMat4            m_viewMatrix;
    JzMat4            m_projectionMatrix;
    JzEProjectionMode m_projectionMode;
    F32               m_fov;
    F32               m_size;
    F32               m_near;
    F32               m_far;
    JzVec3            m_clearColor;
};
} // namespace JzRE