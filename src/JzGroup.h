#pragma once

#include "JzWidget.h"
#include "JzWidgetContainer.h"

namespace JzRE {
/**
 * @brief Group Widget
 */
class JzGroup : public JzWidget, public JzWidgetContainer {
protected:
    /**
     * @brief Draw the widget
     */
    virtual void _Draw_Impl() override;
};
} // namespace JzRE