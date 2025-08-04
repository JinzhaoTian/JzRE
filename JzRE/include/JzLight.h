#pragma once

#include "CommonTypes.h"
#include "OGLShader.h"

namespace JzRE {
/**
 * @brief Light class
 */
class JzLight {
public:
    /**
     * @brief Constructor
     *
     * @param position The position of the light
     * @param color The color of the light
     */
    JzLight(const glm::vec3 &position, const glm::vec3 &color);

    /**
     * @brief Destructor
     */
    virtual ~JzLight() = default;

    /**
     * @brief Set the position of the light
     *
     * @param position The position of the light
     */
    void SetPosition(const glm::vec3 &position);

    /**
     * @brief Get the position of the light
     *
     * @return The position of the light
     */
    const glm::vec3 &GetPosition() const;

    /**
     * @brief Set the color of the light
     * @param color The color of the light
     */
    void SetColor(const glm::vec3 &color);

    /**
     * @brief Get the color of the light
     * @return The color of the light
     */
    const glm::vec3 &GetColor() const;

    /**
     * @brief Apply the light to the shader
     * @param shader The shader to apply the light to
     * @param index The index of the light
     */
    virtual void ApplyLight(std::shared_ptr<OGLShader> shader, int index) const = 0;

protected:
    glm::vec3 position;
    glm::vec3 color;
};
}; // namespace JzRE