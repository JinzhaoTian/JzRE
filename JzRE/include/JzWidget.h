#pragma once

#include "CommonTypes.h"
#include "JzIDrawable.h"
#include "JzIPluginable.h"

namespace JzRE {
/**
 * @brief Base class for all widgets
 */
class JzWidget : public JzIDrawable, public JzIPluginable {
public:
    /**
     * @brief Constructor
     */
    JzWidget();

    /**
     * @brief Draw the widget on the panel
     */
    virtual void Draw() override;

protected:
    /**
     * @brief Implementation of the Draw method
     */
    virtual void _Draw_Impl() = 0;

protected:
    String m_widgetID = "?";

private:
    static U64 __WIDGET_ID_INCREMENT;
};

} // namespace JzRE