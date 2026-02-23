/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <filesystem>
#include <memory>
#include <vector>

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Core/JzVector.h"
#include "JzRE/Runtime/Function/ECS/JzEntity.h"
#include "JzRE/Runtime/Platform/RHI/JzGPUTextureObject.h"
#include "JzRE/Editor/Panels/JzView.h"

namespace JzRE {

/**
 * @brief Preview mode for the asset view
 */
enum class JzEPreviewMode : U8 {
    None,
    Texture,
    Model
};

/**
 * @brief Asset View Panel Window
 *
 * Displays a preview of the selected asset. Textures are shown as 2D images,
 * models are rendered in 3D with an orbit camera.
 */
class JzAssetView : public JzView {
public:
    JzAssetView(const String &name, Bool is_opened);

    void Update(F32 deltaTime) override;

    /**
     * @brief Preview the asset at the given path
     *
     * @param path Absolute path to the asset file
     */
    void PreviewAsset(const std::filesystem::path &path);

    /**
     * @brief Clear the current preview
     */
    void ClearPreview();

protected:
    JzRenderVisibility GetVisibility() const override
    {
        return JzRenderVisibility::Isolated;
    }

    void     _Draw_Impl() override;
    JzEntity GetCameraEntity() override;

private:
    void PreviewTexture(const std::filesystem::path &path);
    void PreviewModel(const std::filesystem::path &path);
    void CleanupPreviewEntities();
    void CreatePreviewCamera();
    void CreatePreviewInputState();
    void UpdatePreviewInputState();

private:
    JzEPreviewMode        m_previewMode = JzEPreviewMode::None;
    std::filesystem::path m_currentPath;

    // Texture preview
    std::shared_ptr<JzGPUTextureObject> m_previewTexture;

    // Model preview
    std::vector<JzEntity> m_previewEntities;
    JzEntity              m_previewCamera = INVALID_ENTITY;

    // Dedicated input state for asset preview (isolated from scene view)
    JzEntity m_previewInputState = INVALID_ENTITY;

    // Orbit camera sensitivity (passed to JzOrbitControllerComponent)
    F32 m_orbitSensitivity = 0.005f;
    F32 m_panSensitivity   = 0.02f;
    F32 m_zoomSensitivity  = 1.0f;
    F32 m_minDistance      = 0.5f;
    F32 m_maxDistance      = 50.0f;

    // Mouse tracking state
    Bool    m_leftMousePressed  = false;
    Bool    m_rightMousePressed = false;
    JzVec2  m_lastMousePos      = JzVec2(0.0f, 0.0f);
    Bool    m_firstMouse        = true;
    JzIVec2 m_lastViewSize      = JzIVec2(0, 0);
};

} // namespace JzRE
