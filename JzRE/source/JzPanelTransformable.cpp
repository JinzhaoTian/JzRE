#include "JzPanelTransformable.h"

JzRE::JzPanelTransformable::JzPanelTransformable()
{
}

void JzRE::JzPanelTransformable::SetPosition() { }

void JzRE::JzPanelTransformable::SetSize() { }

void JzRE::JzPanelTransformable::SetAlignment() { }

void JzRE::JzPanelTransformable::GetPosition() { }

void JzRE::JzPanelTransformable::GetSize() { }

void JzRE::JzPanelTransformable::GetHorizontalAlignment() { }

void JzRE::JzPanelTransformable::GetVerticalAlignment() { }

void JzRE::JzPanelTransformable::Update()
{
    if (!m_firstFrame) {
    }

    m_firstFrame = false;
}