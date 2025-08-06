#pragma once

#include "CommonTypes.h"
#include "JzCamera.h"
#include "JzEditorActions.h"
#include "JzPanelWindow.h"
#include "JzRHIRenderer.h"
#include "JzScene.h"

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
     * @brief Initialize the frame
     */
    virtual void InitFrame();

    /**
     * @brief Draw the frame
     */
    virtual void DrawFrame();

    /**
     * @brief Get the safe size
     *
     * @return The safe size
     */
    std::pair<U16, U16> GetSafeSize() const;

    /**
     * @brief Get the camera
     *
     * @return The camera
     */
    virtual JzCamera *GetCamera() = 0;

    /**
     * @brief Get the scene
     *
     * @return The scene
     */
    virtual JzScene *GetScene() = 0;

protected:
    /**
     * @brief Implementation of the Draw method
     */
    void _Draw_Impl() override;

protected:
    std::unique_ptr<JzRHIRenderer> m_renderer;
};

} // namespace JzRE