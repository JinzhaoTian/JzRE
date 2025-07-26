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

protected:
    /**
     * @brief Destructor
     */
    virtual ~JzIDrawable() = default;
};
} // namespace JzRE
