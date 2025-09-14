/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRenderEngine.h"
#include <memory>
#include "JzContext.h"
#include "JzClock.h"

JzRE::JzRenderEngine::JzRenderEngine(JzERHIType rhiType, std::filesystem::path &openDirectory)
{
    auto &context = JzRE_CONTEXT();
    if (!context.IsInitialized()) {
        context.Initialize(rhiType, openDirectory);
    }

    m_editor = std::make_unique<JzEditor>();
}

JzRE::JzRenderEngine::~JzRenderEngine()
{
    if (m_editor) {
        m_editor.reset();
    }
}

void JzRE::JzRenderEngine::Run()
{
    JzRE::JzClock clock;

    while (IsRunning()) {
        m_editor->PreUpdate();
        m_editor->Update(clock.GetDeltaTime());
        m_editor->PostUpdate();

        clock.Update();
    }
}

JzRE::Bool JzRE::JzRenderEngine::IsRunning() const
{
    return !JzRE_CONTEXT().GetWindow().ShouldClose();
}
