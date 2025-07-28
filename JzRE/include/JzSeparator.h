#pragma once

#include "CommonTypes.h"
#include "JzWidget.h"

namespace JzRE {
/**
 * @brief Separator widget
 */
class JzSeparator : public JzWidget {
protected:
    /**
     * @brief Implementation of the Draw method
     */
    void _Draw_Impl() override;
};
} // namespace JzRE