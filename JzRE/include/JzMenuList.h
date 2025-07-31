#pragma once

#include "CommonTypes.h"
#include "JzEvent.h"
#include "JzGroup.h"

namespace JzRE {
/**
 * @brief Menu List Widget
 */
class JzMenuList : public JzGroup {
public:
    /**
     * @brief Construct a new JzMenuList object
     *
     * @param name
     * @param is_locked
     */
    JzMenuList(const String &name, Bool is_locked = false);

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