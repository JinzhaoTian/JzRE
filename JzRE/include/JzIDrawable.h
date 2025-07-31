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
