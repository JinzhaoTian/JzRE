#pragma once

#include "CommonTypes.h"
#include "JzEAlignment.h"
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
    JzPanelTransformable(JzEHorizontalAlignment defaultHorizontalAlignment = JzEHorizontalAlignment::LEFT,
                         JzEVerticalAlignment   defaultVerticalAlignment   = JzEVerticalAlignment::TOP);

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
    JzEHorizontalAlignment GetHorizontalAlignment() const;

    /**
     * @brief Get the vertical alignment of the panel
     */
    JzEVerticalAlignment GetVerticalAlignment() const;

protected:
    /**
     * @brief Update the panel
     */
    void Update();

    /**
     * @brief Implementation of the Draw method
     */
    virtual void _Draw_Impl() = 0;

private:
    void UpdatePosition();
    void UpdateSize();

public:
    Bool autoSize = true;

protected:
    JzEHorizontalAlignment m_defaultHorizontalAlignment;
    JzEVerticalAlignment   m_defaultVerticalAlignment;
    JzEHorizontalAlignment m_horizontalAlignment = JzEHorizontalAlignment::LEFT;
    JzEVerticalAlignment   m_verticalAlignment   = JzEVerticalAlignment::TOP;
    Bool                   m_firstFrame          = true;
    Bool                   m_positionChanged     = false;
    Bool                   m_sizeChanged         = false;
    Bool                   m_alignmentChanged    = false;
};

} // namespace JzRE