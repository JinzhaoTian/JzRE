/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Core/JzRETypes.h"
#include "JzRE/Core/JzEvent.h"
#include "JzRE/UI/JzText.h"

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