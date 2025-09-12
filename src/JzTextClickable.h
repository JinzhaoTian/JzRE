#pragma once

#include "JzRETypes.h"
#include "JzText.h"
#include "JzEvent.h"

namespace JzRE {
/**
 * @brief Clickable Text Widget
 */
class JzTextClickable : public JzText {
public:
    /**
     * @brief Constructor
     *
     * @param p_content
     */
    JzTextClickable(const String &p_content = "");

protected:
    /**
     * @brief Implementation of the Draw method
     */
    void _Draw_Impl() override;

public:
    JzEvent<> ClickedEvent;
    JzEvent<> DoubleClickedEvent;
};

} // namespace JzRE