#include "JzRenderEngine.h"

JzRE::JzRenderEngine::JzRenderEngine() :
    m_context("", ""),
    m_editor(m_context) { }

JzRE::JzRenderEngine::~JzRenderEngine() { }

void JzRE::JzRenderEngine::Run()
{
    JzRE::JzClock clock;

    while (IsRunning()) {
        m_editor.PreUpdate();
        m_editor.Update(clock.GetDeltaTime());
        m_editor.PostUpdate();

        clock.Update();
    }
}

JzRE::Bool JzRE::JzRenderEngine::IsRunning() const
{
    return !m_context.window->ShouldClose();
}
