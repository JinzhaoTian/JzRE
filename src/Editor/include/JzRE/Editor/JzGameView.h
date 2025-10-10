/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Core/JzRETypes.h"
#include "JzRE/Editor/JzView.h"

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

    /**
     * @brief Update the scene view
     *
     * @param deltaTime
     */
    void Update(F32 deltaTime) override;
};
} // namespace JzRE