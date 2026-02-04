/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <memory>

#include "JzRE/Editor/UI/JzFrame.h"
#include "JzRE/Editor/UI/JzPanelWindow.h"
#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Function/ECS/JzEntity.h"
#include "JzRE/Runtime/Function/Rendering/JzRenderTarget.h"
#include "JzRE/Runtime/Function/Rendering/JzRenderTargetRegistry.h"

namespace JzRE {

/**
 * @brief Base class for all view panels.
 *
 * Views register their render targets with RenderSystem during construction.
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
     * @brief Whether to include editor-only entities (JzEditorOnlyTag).
     *
     * Override in subclasses. Default is false.
     * SceneView should return true.
     */
    virtual Bool IncludeEditorOnly() const { return false; }

    /**
     * @brief Whether to include preview-only entities (JzPreviewOnlyTag).
     *
     * Override in subclasses. Default is false.
     * AssetView should return true.
     */
    virtual Bool IncludePreviewOnly() const { return false; }

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
    String                              m_name;
    JzFrame                            *m_frame;
    std::unique_ptr<JzRenderTarget>     m_renderTarget;
    JzRenderTargetRegistry::Handle      m_registryHandle = JzRenderTargetRegistry::INVALID_HANDLE;
};

} // namespace JzRE
