/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Editor/JzGameView.h"
#include "JzRE/Editor/JzEditor.h"
#include "JzRE/Editor/JzSceneManager.h"

JzRE::JzGameView::JzGameView(const String &name, Bool is_opened) :
    JzRE::JzView(name, is_opened)
{
    m_renderer = std::make_unique<JzRHIRenderer>();
}
