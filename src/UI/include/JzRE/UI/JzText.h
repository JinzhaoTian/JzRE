/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRETypes.h"
#include "JzDataWidget.h"

namespace JzRE {
/**
 * @brief Text Widget
 */
class JzText : public JzDataWidget<String> {
public:
    /**
     * @brief Constructor
     *
     * @param p_content
     */
    JzText(const String &p_content = "");

protected:
    /**
     * @brief Implementation of the Draw method
     */
    void _Draw_Impl() override;

public:
    String content;
};
} // namespace JzRE