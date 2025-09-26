/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Core/JzVector.h"

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
    JzLight(const JzVec3 &position, const JzVec3 &color);

    /**
     * @brief Destructor
     */
    virtual ~JzLight() = default;

    /**
     * @brief Set the position of the light
     *
     * @param position The position of the light
     */
    void SetPosition(const JzVec3 &position);

    /**
     * @brief Get the position of the light
     *
     * @return The position of the light
     */
    const JzVec3 &GetPosition() const;

    /**
     * @brief Set the color of the light
     * @param color The color of the light
     */
    void SetColor(const JzVec3 &color);

    /**
     * @brief Get the color of the light
     * @return The color of the light
     */
    const JzVec3 &GetColor() const;

protected:
    JzVec3 position;
    JzVec3 color;
};

}; // namespace JzRE