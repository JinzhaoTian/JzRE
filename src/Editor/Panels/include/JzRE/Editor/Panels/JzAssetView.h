/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Editor/Panels/JzView.h"

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

protected:
    /**
     * @brief AssetView only renders preview entities.
     */
    JzRenderVisibility GetVisibility() const override
    {
        return JzRenderVisibility::PreviewOnly;
    }

    String GetPassName() const override
    {
        return "AssetPreviewPass";
    }

    String GetOutputName() const override
    {
        return "AssetPreview_Color";
    }
};
} // namespace JzRE
