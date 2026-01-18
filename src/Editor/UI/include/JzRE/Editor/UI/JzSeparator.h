/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Editor/UI/JzWidget.h"

namespace JzRE {

/**
 * @brief Separator Widget
 */
class JzSeparator : public JzWidget {
protected:
    /**
     * @brief Implementation of the Draw method
     */
    void _Draw_Impl() override;
};

} // namespace JzRE