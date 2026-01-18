/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Core/JzEvent.h"
#include "JzRE/Editor/UI/JzText.h"

namespace JzRE {

/**
 * @brief Clickable Text Widget
 */
class JzTextClickable : public JzText {
public:
    /**
     * @brief Constructor
     *
     * @param content
     */
    JzTextClickable(const String &content = "");

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