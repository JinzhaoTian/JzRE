#pragma once

#include "CommonTypes.h"
#include "IJzDrawable.h"

namespace JzRE {
/**
 * @brief A widget
 */
class JzWidget : public IJzDrawable {
public:
    /**
     * @brief Constructor
     */
    JzWidget() = default;

    /**
     * @brief Destructor
     */
    virtual ~JzWidget() = default;
};

} // namespace JzRE