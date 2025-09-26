/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Core/JzRETypes.h"
#include "JzRE/UI/JzWidget.h"

namespace JzRE {
/**
 * @brief Spacing Widget
 */
class JzSpacing : public JzWidget {
public:
    /**
     * Constructor
     * @param p_spaces
     */
    JzSpacing(U16 spaces = 1);

protected:
    /**
     * @brief Implementation of the Draw method
     */
    void _Draw_Impl() override;

public:
    U16 spaces = 1;
};
} // namespace JzRE