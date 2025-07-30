#pragma once

#include "CommonTypes.h"
#include "JzCamera.h"
#include "JzPanelWindow.h"
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

    void Render();

    virtual void Update(F32 deltaTime);

    virtual void InitFrame();

    virtual void DrawFrame();

    std::pair<U16, U16> GetSafeSize() const;

    virtual JzCamera *GetCamera() = 0;

    virtual JzScene *GetScene() = 0;

protected:
    /**
     * @brief Implementation of the Draw method
     */
    void _Draw_Impl() override;
};

} // namespace JzRE