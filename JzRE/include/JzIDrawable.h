#pragma once

namespace JzRE {
/**
 * @brief Interface for drawable objects
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
