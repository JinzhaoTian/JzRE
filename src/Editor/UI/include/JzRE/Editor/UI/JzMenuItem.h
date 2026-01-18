/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Core/JzEvent.h"
#include "JzRE/Editor/UI/JzDataWidget.h"

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
     * @param isCheckable
     * @param isChecked
     */
    JzMenuItem(const String &name, const String &shortcut = "", Bool isCheckable = false, Bool isChecked = false);

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