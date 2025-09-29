/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/App/JzREInstance.h"
#include "JzRE/Core/JzClock.h"
#include "JzRE/Editor/JzContext.h"
#include "JzRE/Editor/JzWindow.h"

// GLFW needs to be included for context management
#include <GLFW/glfw3.h>

JzRE::JzREInstance::JzREInstance(JzERHIType rhiType,
                                 std::filesystem::path &openDirectory) {
  auto &context = JzContext::GetInstance();
  if (!context.IsInitialized()) {
    context.Initialize(rhiType, openDirectory);
  }

  m_editor = std::make_unique<JzEditor>();

  // Provide the frontend to the context so other systems can access it.
  context.SetRenderFrontend(&m_renderFrontend);

  // Initialize and start the render thread
  m_renderBackend = CreateRef<JzRenderBackend>(
      m_renderFrontend, &JzContext::GetInstance().GetWindow());

  // Detach the graphics context from the main thread so the render thread can
  // acquire it.
  glfwMakeContextCurrent(nullptr);

  m_renderBackend->Start();
}

JzRE::JzREInstance::~JzREInstance() {
  // Stop the render thread before destroying the editor and context
  if (m_renderBackend) {
    m_renderBackend->Stop();
  }

  if (m_editor) {
    m_editor.reset();
  }
}

void JzRE::JzREInstance::Run() {
  JzRE::JzClock clock;

  while (IsRunning()) {
    // Begin a new frame. This will wait for the render thread to be ready.
    m_renderFrontend.BeginFrame();

    // --- All logic and command submission happens here ---
    m_editor->PreUpdate();
    m_editor->Update(clock.GetDeltaTime()); // This should now submit render commands
    m_editor->PostUpdate(); // This might submit UI render commands

    clock.Update();
    // --- End of logic and command submission ---

    // Present the frame. This submits all commands to the render thread.
    m_renderFrontend.Present();
  }
}

JzRE::Bool JzRE::JzREInstance::IsRunning() const {
  return !JzContext::GetInstance().GetWindow().ShouldClose();
}
