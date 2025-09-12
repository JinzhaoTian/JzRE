#pragma once

#include "JzRETypes.h"
#include "JzEvent.h"
#include "JzWidget.h"

namespace JzRE {

enum class JzEArrowDirection : U8 {
    NONE  = 0,
    LEFT  = 1,
    RIGHT = 2,
    UP    = 3,
    DOWN  = 4,
};

enum class JzEAlign : U8 {
    LEFT   = 0,
    CENTER = 1,
    RIGHT  = 2,
};

/**
 * @brief Arrow Button Widget
 */
class JzArrowButton : public JzWidget {
public:
    /**
     * @brief Constructor
     *
     * @param p_direction
     */
    JzArrowButton(JzEArrowDirection p_direction = JzEArrowDirection::NONE, JzEAlign p_align = JzEAlign::CENTER);

protected:
    /**
     * @brief Implementation of the Draw method
     */
    void _Draw_Impl() override;

public:
    /**
     * @brief Event triggered when the button is clicked
     */
    JzEvent<> ClickedEvent;

public:
    Bool              disabled = false;
    JzEArrowDirection arrowDirection;
    JzEAlign          buttonAlign;
};

} // namespace JzRE