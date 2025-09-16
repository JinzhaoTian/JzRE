/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzWidget.h"
#include "JzWidgetContainer.h"
#include "JzEAlignment.h"
#include "JzVector.h"

namespace JzRE {

/**
 * @brief Group Widget
 */
class JzGroup : public JzWidget, public JzWidgetContainer {
public:
    /**
     * @brief Constructor
     *
     * @param p_horizontalAlignment
     * @param p_size
     */
    JzGroup(JzEHorizontalAlignment p_horizontalAlignment = JzEHorizontalAlignment::LEFT, JzVec2 p_size = {150.f, 0.f}, JzVec2 p_spacing = {8.f, 4.f});

protected:
    /**
     * @brief Draw the widget
     */
    virtual void _Draw_Impl() override;

public:
    JzVec2                 size;
    JzVec2                 spacing;
    JzEHorizontalAlignment horizontalAlignment;
};

} // namespace JzRE