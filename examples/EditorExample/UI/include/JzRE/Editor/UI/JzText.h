/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Editor/UI/JzDataWidget.h"

namespace JzRE {

/**
 * @brief Text Widget
 */
class JzText : public JzDataWidget<String> {
public:
    /**
     * @brief Constructor
     *
     * @param content
     */
    JzText(const String &content = "");

protected:
    /**
     * @brief Implementation of the Draw method
     */
    void _Draw_Impl() override;

public:
    String content;
};

} // namespace JzRE
