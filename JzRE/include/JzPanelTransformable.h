#pragma once

#include "CommonTypes.h"
#include "JzPanel.h"

namespace JzRE {
/**
 * @brief Base class for all transformable panels
 */
class JzPanelTransformable : public JzPanel {
public:
    /**
     * @brief Constructor
     */
    JzPanelTransformable();

    /**
     * @brief Set the position of the panel
     */
    void SetPosition();

    /**
     * @brief Set the size of the panel
     */
    void SetSize();

    /**
     * @brief Set the alignment of the panel
     */
    void SetAlignment();

    /**
     * @brief Get the position of the panel
     */
    void GetPosition();

    /**
     * @brief Get the size of the panel
     */
    void GetSize();

    /**
     * @brief Get the horizontal alignment of the panel
     */
    void GetHorizontalAlignment();

    /**
     * @brief Get the vertical alignment of the panel
     */
    void GetVerticalAlignment();

protected:
    /**
     * @brief Update the panel
     */
    void Update();

    /**
     * @brief Implementation of the Draw method
     */
    virtual void _Draw_Impl() = 0;

public:
    Bool autoSize = true;

protected:
    Bool m_firstFrame = true;
};

} // namespace JzRE