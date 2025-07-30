#include "JzPanelTransformable.h"

JzRE::JzPanelTransformable::JzPanelTransformable(JzRE::JzEHorizontalAlignment defaultHorizontalAlignment,
                                                 JzRE::JzEVerticalAlignment   defaultVerticalAlignment) :
    m_defaultHorizontalAlignment(defaultHorizontalAlignment),
    m_defaultVerticalAlignment(defaultVerticalAlignment) { }

void JzRE::JzPanelTransformable::SetPosition() { }

void JzRE::JzPanelTransformable::SetSize() { }

void JzRE::JzPanelTransformable::SetAlignment() { }

void JzRE::JzPanelTransformable::GetPosition() { }

void JzRE::JzPanelTransformable::GetSize() { }

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
        UpdatePosition();
    }

    m_firstFrame = false;
}

void JzRE::JzPanelTransformable::UpdateSize()
{
    if (m_sizeChanged) {
        ImVec2 size; // TODO
        ImGui::SetWindowSize(size, ImGuiCond_Always);
        m_sizeChanged = false;
    }
}

void JzRE::JzPanelTransformable::UpdatePosition()
{
    ImVec2 offsettedDefaultPos; // TODO
    ImGui::SetWindowPos(offsettedDefaultPos, ImGuiCond_Once);

    if (m_positionChanged || m_alignmentChanged) {
        ImVec2 offsettedPos;
        ImGui::SetWindowPos(offsettedPos, ImGuiCond_Always);
        m_positionChanged  = false;
        m_alignmentChanged = false;
    }
}