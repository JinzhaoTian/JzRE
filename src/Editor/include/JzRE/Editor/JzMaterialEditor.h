/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRETypes.h"
#include "JzPanelWindow.h"

namespace JzRE {
/**
 * @brief Material Edit Panel Window
 */
class JzMaterialEditor : public JzPanelWindow {
public:
    /**
     * @brief Constructor
     *
     * @param name
     * @param is_opened
     */
    JzMaterialEditor(const String &name, Bool is_opened);
};
} // namespace JzRE