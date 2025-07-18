#pragma once

namespace JzRE {
/**
 * @brief Interface for drawable objects
 */
class IJzDrawable {
public:
    /**
     * @brief Draw the object
     */
    virtual void Draw() = 0;

protected:
    /**
     * @brief Destructor
     */
    virtual ~IJzDrawable() = default;
};
} // namespace JzRE
