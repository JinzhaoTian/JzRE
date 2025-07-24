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
    JzWidget();

    /**
     * @brief Draw the widget on the panel
     */
    virtual void Draw() override;

protected:
    virtual void _Draw_Impl() = 0;

protected:
    String m_widgetID = "?";

private:
    static U64 __WIDGET_ID_INCREMENT;
};

} // namespace JzRE