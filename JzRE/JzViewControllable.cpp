#include "JzViewControllable.h"

JzRE::JzViewControllable::JzViewControllable(const JzRE::String &name, JzRE::Bool is_opened) :
    JzRE::JzView(name, is_opened),
    m_cameraController(*this, m_camera)
{
    ResetCameraTransform();
}

void JzRE::JzViewControllable::Update(JzRE::F32 deltaTime)
{
    m_cameraController.HandleInputs(deltaTime);
    JzView::Update(deltaTime);
}

void JzRE::JzViewControllable::InitFrame()
{
    JzView::InitFrame();

    // m_renderer->AddDescriptor<Rendering::GridRenderPass::GridDescriptor>({
    // 	m_gridColor,
    // 	m_camera.GetPosition()
    // });
}

void JzRE::JzViewControllable::ResetCameraTransform()
{
    // m_camera.transform->SetWorldPosition(kDefaultCameraPosition);
    // m_camera.transform->SetWorldRotation(kDefaultCameraRotation);
}

JzRE::JzCameraController &JzRE::JzViewControllable::GetCameraController()
{
    return m_cameraController;
}

JzRE::JzCamera *JzRE::JzViewControllable::GetCamera()
{
    return &m_camera;
}

void JzRE::JzViewControllable::ResetClearColor()
{
    m_camera.SetClearColor({});
}