/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Core/JzRETypes.h"
#include "JzRE/Core/JzEvent.h"
#include "JzRE/UI/JzDataWidget.h"

namespace JzRE {
/**
 * @brief Input Text Widget
 */
class JzInputText : public JzDataWidget<JzRE::String> {
public:
    /**
     * @brief Constructor
     *
     * @param p_content
     */
    JzInputText(const String &p_content = "", const String &p_label = "");

protected:
    /**
     * @brief Implementation of the Draw method
     */
    void _Draw_Impl() override;

public:
    String          content;
    String          label;
    Bool            selectAllOnClick = false;
    F32             width            = 200.0f;
    JzEvent<String> ContentChangedEvent;
    JzEvent<String> EnterPressedEvent;
};

} // namespace JzRE