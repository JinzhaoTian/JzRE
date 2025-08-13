#include "JzPanelTransformable.h"

JzRE::JzPanelTransformable::JzPanelTransformable(const JzRE::JzVec2          &defaultPosition,
                                                 const JzRE::JzVec2          &defaultSize,
                                                 JzRE::JzEHorizontalAlignment defaultHorizontalAlignment,
                                                 JzRE::JzEVerticalAlignment   defaultVerticalAlignment) :
    m_defaultPosition(defaultPosition),
    m_defaultSize(defaultSize),
    m_defaultHorizontalAlignment(defaultHorizontalAlignment),
    m_defaultVerticalAlignment(defaultVerticalAlignment) { }

void JzRE::JzPanelTransformable::SetPosition(const JzRE::JzVec2 &position)
{
    m_position        = position;
    m_positionChanged = true;
}

void JzRE::JzPanelTransformable::SetSize(const JzRE::JzVec2 &size)
{
    m_size        = size;
    m_sizeChanged = true;
}

void JzRE::JzPanelTransformable::SetAlignment(JzRE::JzEHorizontalAlignment horizontalAlignment, JzRE::JzEVerticalAlignment verticalAlignment)
{
    m_horizontalAlignment = horizontalAlignment;
    m_verticalAlignment   = verticalAlignment;
    m_alignmentChanged    = true;
}

const JzRE::JzVec2 &JzRE::JzPanelTransformable::GetPosition() const
{
    return m_position;
}

const JzRE::JzVec2 &JzRE::JzPanelTransformable::GetSize() const
{
    return m_size;
}

JzRE::JzEHorizontalAlignment JzRE::JzPanelTransformable::GetHorizontalAlignment() const
{
    return m_horizontalAlignment;
}

JzRE::JzEVerticalAlignment JzRE::JzPanelTransformable::GetVerticalAlignment() const
{
    return m_verticalAlignment;
}

void JzRE::JzPanelTransformable::Update()
{
    if (!m_firstFrame) {
        if (!autoSize)
            UpdateSize();
        CopyImGuiSize();

        UpdatePosition();
        CopyImGuiPosition();
    }

    m_firstFrame = false;
}

void JzRE::JzPanelTransformable::UpdatePosition()
{
    if (m_defaultPosition.x() != -1.f && m_defaultPosition.y() != 1.f) {
        JzVec2 offsettedDefaultPos = m_defaultPosition + CalculatePositionAlignmentOffset(true);
        ImGui::SetWindowPos(JzRE::JzConverter::ToImVec2(offsettedDefaultPos), ImGuiCond_Once);
    }

    if (m_positionChanged || m_alignmentChanged) {
        JzVec2 offset = CalculatePositionAlignmentOffset(false);
        JzVec2 offsetPos(m_position.x() + offset.x(), m_position.y() + offset.y());
        ImGui::SetWindowPos(JzConverter::ToImVec2(offsetPos), ImGuiCond_Always);
        m_positionChanged  = false;
        m_alignmentChanged = false;
    }
}

void JzRE::JzPanelTransformable::UpdateSize()
{
    if (m_sizeChanged) {
        ImGui::SetWindowSize(JzConverter::ToImVec2(m_size), ImGuiCond_Always);
        m_sizeChanged = false;
    }
}

void JzRE::JzPanelTransformable::CopyImGuiPosition()
{
    m_position = JzConverter::ToJzVec2(ImGui::GetWindowPos());
}

void JzRE::JzPanelTransformable::CopyImGuiSize()
{
    m_size = JzConverter::ToJzVec2(ImGui::GetWindowSize());
}

JzRE::JzVec2 JzRE::JzPanelTransformable::CalculatePositionAlignmentOffset(JzRE::Bool p_default)
{
    JzVec2 result(0.0f, 0.0f);

    switch (p_default ? m_defaultHorizontalAlignment : m_horizontalAlignment) {
        case JzEHorizontalAlignment::LEFT:
            // No offset needed for left alignment
            break;
        case JzEHorizontalAlignment::CENTER:
            result.x() -= m_size.x() / 2.0f;
            break;
        case JzEHorizontalAlignment::RIGHT:
            result.x() -= m_size.x();
            break;
    }

    switch (p_default ? m_defaultVerticalAlignment : m_verticalAlignment) {
        case JzEVerticalAlignment::TOP:
            // No offset needed for top alignment
            break;
        case JzEVerticalAlignment::MIDDLE:
            result.y() -= m_size.y() / 2.0f;
            break;
        case JzEVerticalAlignment::BOTTOM:
            result.y() -= m_size.y();
            break;
    }

    return result;
}