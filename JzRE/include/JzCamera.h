#pragma once

#include "CommonTypes.h"
#include "JzEntity.h"

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
     * @brief Set the field of view
     *
     * @param p_value The field of view
     */
    void SetFov(F32 p_value);

    /**
     * @brief Get the field of view
     *
     * @return The field of view
     */
    F32 GetFov() const;

    /**
     * @brief Set the far plane
     *
     * @param p_value The far plane
     */
    void SetFar(F32 p_value);

    /**
     * @brief Get the far plane
     *
     * @return The far plane
     */
    F32 GetFar() const;

    /**
     * @brief Set the clear color
     *
     * @param p_value The clear color
     */
    void SetClearColor();

private:
    F32 m_fov;
    F32 m_size;
    F32 m_near;
    F32 m_far;
};
} // namespace JzRE