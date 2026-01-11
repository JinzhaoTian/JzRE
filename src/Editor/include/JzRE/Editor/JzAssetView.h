/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Editor/JzView.h"

namespace JzRE {
/**
 * @brief Asset View Panel Window
 */
class JzAssetView : public JzView {
public:
    /**
     * @brief Constructor
     *
     * @param name
     * @param is_opened
     */
    JzAssetView(const String &name, Bool is_opened);

    /**
     * @brief Update the asset view
     *
     * @param deltaTime
     */
    void Update(F32 deltaTime) override;
};
} // namespace JzRE