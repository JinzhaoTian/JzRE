#pragma once

#include "CommonTypes.h"
#include "JzDataWidget.h"
#include "JzEvent.h"

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
    JzEvent<String> ContentChangedEvent;
    JzEvent<String> EnterPressedEvent;
};

} // namespace JzRE