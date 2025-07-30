#pragma once

#include "CommonTypes.h"
#include "JzConverter.h"
#include "JzEAlignment.h"
#include "JzPanel.h"
#include "JzVector.h"

namespace JzRE {
/**
 * @brief Base class for all transformable panels
 */
class JzPanelTransformable : public JzPanel {
public:
    /**
     * @brief Constructor
     */
    JzPanelTransformable(
        const JzVec2          &defaultPosition            = {-1.f, -1.f},
        const JzVec2          &defaultSize                = {-1.f, -1.f},
        JzEHorizontalAlignment defaultHorizontalAlignment = JzEHorizontalAlignment::LEFT,
        JzEVerticalAlignment   defaultVerticalAlignment   = JzEVerticalAlignment::TOP);

    /**
     * @brief Set the position of the panel
     */
    void SetPosition(const JzVec2 &position);

    /**
     * @brief Set the size of the panel
     */
    void SetSize(const JzVec2 &size);

    /**
     * @brief Set the alignment of the panel
     */
    void SetAlignment(JzEHorizontalAlignment horizontalAlignment, JzEVerticalAlignment verticalAligment);

    /**
     * @brief Get the position of the panel
     */
    const JzVec2 &GetPosition() const;

    /**
     * @brief Get the size of the panel
     */
    const JzVec2 &GetSize() const;

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
    JzVec2 CalculatePositionAlignmentOffset(Bool p_default = false);
    void   UpdatePosition();
    void   UpdateSize();

public:
    Bool autoSize = true;

protected:
    JzVec2                 m_defaultPosition;
    JzVec2                 m_defaultSize;
    JzEHorizontalAlignment m_defaultHorizontalAlignment;
    JzEVerticalAlignment   m_defaultVerticalAlignment;
    JzVec2                 m_position            = {0.0f, 0.0f};
    JzVec2                 m_size                = {0.0f, 0.0f};
    Bool                   m_positionChanged     = false;
    Bool                   m_sizeChanged         = false;
    JzEHorizontalAlignment m_horizontalAlignment = JzEHorizontalAlignment::LEFT;
    JzEVerticalAlignment   m_verticalAlignment   = JzEVerticalAlignment::TOP;
    Bool                   m_alignmentChanged    = false;
    Bool                   m_firstFrame          = true;
};

} // namespace JzRE