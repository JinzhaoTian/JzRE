/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRETypes.h"
#include "JzView.h"

namespace JzRE {
/**
 * @brief Game View Panel Window
 */
class JzGameView : public JzView {
public:
    /**
     * @brief Constructor
     *
     * @param name
     * @param is_opened
     */
    JzGameView(const String &name, Bool is_opened);
};
} // namespace JzRE