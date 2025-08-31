#pragma once

#include "CommonTypes.h"
#include "JzDataWidget.h"

namespace JzRE {
/**
 * @brief Text Widget
 */
class JzText : public JzDataWidget<JzRE::String> {
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