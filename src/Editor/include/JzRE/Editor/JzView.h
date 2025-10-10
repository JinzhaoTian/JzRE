/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Core/JzRETypes.h"
#include "JzRE/UI/JzFrame.h"
#include "JzRE/UI/JzPanelWindow.h"

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

protected:
    JzFrame *m_frame;
};

} // namespace JzRE