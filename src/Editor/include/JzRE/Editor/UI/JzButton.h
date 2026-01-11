/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Core/JzEvent.h"
#include "JzRE/Runtime/Core/JzVector.h"
#include "JzRE/Editor/UI/JzWidget.h"

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
     * @param isDisabled Whether the button is disabled
     */
    JzButton(const String &label = "", Bool isDisabled = false);

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
    JzVec2 buttonSize;
    String buttonIdleColor;
    String buttonHoveredColor;
    String buttonClickedColor;
    String buttonLabelColor;
};
} // namespace JzRE