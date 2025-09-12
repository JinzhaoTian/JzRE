/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

namespace JzRE {
/**
 * @brief Interface for Drawable Objects
 */
class JzIDrawable {
public:
    /**
     * @brief Draw the object
     */
    virtual void Draw() = 0;

    /**
     * @brief Destructor
     */
    virtual ~JzIDrawable() = default;
};
} // namespace JzRE
