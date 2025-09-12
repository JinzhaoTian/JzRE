/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRETypes.h"
#include "JzEvent.h"
#include "JzWidget.h"
#include "JzVector.h"

namespace JzRE {
/**
 * @brief Button Widget
 */
class JzButton : public JzWidget {
public:
    /**
     * @brief Constructor
     *
     * @param label The label of the button
     * @param p_disabled Whether the button is disabled
     */
    JzButton(const String &label = "", Bool p_disabled = false);

protected:
    /**
     * @brief Implementation of the Draw method
     */
    void _Draw_Impl() override;

public:
    /**
     * @brief Event triggered when the button is clicked
     */
    JzEvent<> ClickedEvent;

public:
    String label;
    Bool   disabled = false;
    JzVec2 size;
    JzVec4 idleBackgroundColor;
    JzVec4 hoveredBackgroundColor;
    JzVec4 clickedBackgroundColor;
    JzVec4 textColor;
};
} // namespace JzRE