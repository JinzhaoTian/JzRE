#include "JzView.h"

JzRE::JzView::JzView(const JzRE::String &name, JzRE::Bool is_opened) :
    JzRE::JzPanelWindow(name, is_opened)
{
}

void JzRE::JzView::Update(JzRE::F32 deltaTime) { }

void JzRE::JzView::_Draw_Impl() { }

void JzRE::JzView::DrawFrame()
{
    // m_renderer->DrawFrame();
}