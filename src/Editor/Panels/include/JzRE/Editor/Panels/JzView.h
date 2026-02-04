/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <memory>

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Function/ECS/JzEntity.h"
#include "JzRE/Runtime/Function/Rendering/JzRenderTarget.h"
#include "JzRE/Editor/UI/JzFrame.h"
#include "JzRE/Editor/UI/JzPanelWindow.h"

namespace JzRE {

/**
 * @brief Base class for all view panels
 */
class JzView : public JzPanelWindow {
public:
    /**
     * @brief Constructor
     *
     * @param name
     * @param is_opened
     */
    JzView(const String &name, Bool is_opened);

    /**
     * @brief Render the view
     */
    void Render();

    /**
     * @brief Update the view
     */
    virtual void Update(F32 deltaTime);

    /**
     * @brief Get the safe size
     *
     * @return The safe size
     */
    JzIVec2 GetSafeSize() const;

protected:
    /**
     * @brief Implementation of the Draw method
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

protected:
    JzFrame                        *m_frame;
    std::unique_ptr<JzRenderTarget> m_renderTarget;
};

} // namespace JzRE