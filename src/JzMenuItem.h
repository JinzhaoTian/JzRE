#pragma once

#include "JzRETypes.h"
#include "JzDataWidget.h"
#include "JzEvent.h"

namespace JzRE {
/**
 * @brief Menu Item Widget
 */
class JzMenuItem : public JzDataWidget<Bool> {
public:
    /**
     * @brief Construct a new JzMenuItem object
     *
     * @param name
     * @param shortcut
     * @param is_checkable
     * @param is_checked
     */
    JzMenuItem(const String &name, const String &shortcut = "", Bool is_checkable = false, Bool is_checked = false);

protected:
    /**
     * @brief Draw the widget
     */
    void _Draw_Impl() override;

public:
    String        name;
    String        shortcut;
    Bool          checkable;
    Bool          checked;
    JzEvent<>     ClickedEvent;
    JzEvent<Bool> ValueChangedEvent;

private:
    Bool m_selected;
};
} // namespace JzRE