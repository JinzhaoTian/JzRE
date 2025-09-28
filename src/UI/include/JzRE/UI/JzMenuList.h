/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Core/JzRETypes.h"
#include "JzRE/Core/JzEvent.h"
#include "JzRE/UI/JzGroup.h"

namespace JzRE {

/**
 * @brief Menu List Widget
 */
class JzMenuList : public JzGroup {
public:
    /**
     * @brief Constructor
     *
     * @param name
     * @param isLocked
     */
    JzMenuList(const String &name, Bool isLocked = false);

protected:
    /**
     * @brief Draw the widget
     */
    virtual void _Draw_Impl() override;

public:
    String    name;
    Bool      locked;
    JzEvent<> ClickedEvent;

private:
    Bool m_opened;
};

} // namespace JzRE