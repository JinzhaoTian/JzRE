/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzGameView.h"

JzRE::JzGameView::JzGameView(const String &name, Bool is_opened) :
    JzRE::JzView(name, is_opened)
{
    m_renderer = std::make_unique<JzRHIRenderer>();
}
