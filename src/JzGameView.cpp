#include "JzGameView.h"

JzRE::JzGameView::JzGameView(const String &name, Bool is_opened) :
    JzRE::JzView(name, is_opened)
{
    m_renderer = std::make_unique<JzRHIRenderer>();
}
