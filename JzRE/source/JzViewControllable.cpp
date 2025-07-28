#include "JzViewControllable.h"

JzRE::JzViewControllable::JzViewControllable(const JzRE::String &name, JzRE::Bool is_opened) :
    JzRE::JzView(name, is_opened),
    m_cameraController(*this, m_camera) { }

void JzRE::JzViewControllable::Update(JzRE::F32 deltaTime)
{
    m_cameraController.HandleInputs(deltaTime);
    JzView::Update(deltaTime);
}