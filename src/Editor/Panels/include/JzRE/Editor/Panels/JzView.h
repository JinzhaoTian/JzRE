/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Editor/UI/JzFrame.h"
#include "JzRE/Editor/UI/JzPanelWindow.h"
#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Function/ECS/JzEntity.h"
#include "JzRE/Runtime/Function/ECS/JzRenderSystem.h"

namespace JzRE {

/**
 * @brief Base class for all view panels.
 *
 * Views register their render targets with RenderSystem lazily (first draw/update).
 * RenderSystem renders all registered targets during World::Update().
 * Views only display the pre-rendered texture in their _Draw_Impl().
 */
class JzView : public JzPanelWindow {
public:
    /**
     * @brief Constructor
     *
     * @param name Panel name
     * @param is_opened Initial open state
     */
    JzView(const String &name, Bool is_opened);

    /**
     * @brief Destructor - unregisters render target
     */
    ~JzView() override;

    /**
     * @brief Update the view logic (e.g., camera control)
     *
     * @param deltaTime Delta time in seconds
     */
    virtual void Update(F32 deltaTime);

    /**
     * @brief Get the safe size (accounting for title bar)
     *
     * @return The safe size
     */
    JzIVec2 GetSafeSize() const;

protected:
    /**
     * @brief Implementation of the Draw method.
     *
     * Displays the pre-rendered texture from the render target.
     */
    void _Draw_Impl() override;

    /**
     * @brief Get the camera entity to use for rendering.
     *
     * Override in subclasses to use a specific camera.
     *
     * @return The camera entity, or INVALID_ENTITY for main camera
     */
    virtual JzEntity GetCameraEntity();

    /**
     * @brief Get the visibility mask for entity filtering.
     *
     * Override in subclasses to control which entities are rendered:
     * - GameView: MainScene (default)
     * - SceneView: MainScene | Overlay
     * - AssetView: Isolated
     */
    virtual JzRenderVisibility GetVisibility() const
    {
        return JzRenderVisibility::MainScene;
    }

    /**
     * @brief Get per-target render feature flags.
     *
     * Override in subclasses to enable features:
     * - Skybox
     * - Axis
     * - Grid
     * - Manipulator
     */
    virtual JzRenderTargetFeatures GetRenderFeatures() const
    {
        return JzRenderTargetFeatures::None;
    }

    /**
     * @brief Register this view's render target with RenderSystem.
     *
     * Called during construction if the view is opened.
     */
    void RegisterRenderTarget();

    /**
     * @brief Unregister this view's render target from RenderSystem.
     *
     * Called during destruction.
     */
    void UnregisterRenderTarget();

private:
    /**
     * @brief Update the frame widget with the rendered texture.
     */
    void UpdateFrameTexture();

protected:
    String                             m_name;
    JzFrame                           *m_frame;
    JzRenderTargetHandle               m_renderTargetHandle = INVALID_RENDER_TARGET_HANDLE;
};

} // namespace JzRE
