/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/App/JzREInstance.h"
#include "JzRE/Core/JzClock.h"

JzRE::JzREInstance::JzREInstance(JzERHIType rhiType, std::filesystem::path &openDirectory)
{
    auto &context = JzContext::GetInstance();
    if (!context.IsInitialized()) {
        context.Initialize(rhiType, openDirectory);
    }

    m_editor = std::make_unique<JzEditor>();
}

JzRE::JzREInstance::~JzREInstance()
{
    if (m_editor) {
        m_editor.reset();
    }
}

void JzRE::JzREInstance::Run()
{
    JzRE::JzClock clock;

    while (IsRunning()) {
        m_editor->PreUpdate();
        m_editor->Update(clock.GetDeltaTime());
        m_editor->PostUpdate();

        clock.Update();
    }
}

JzRE::Bool JzRE::JzREInstance::IsRunning() const
{
    return !JzContext::GetInstance().GetWindow().ShouldClose();
}
