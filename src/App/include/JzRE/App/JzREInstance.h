/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Core/JzCore.h"
#include "JzRE/Editor/JzEditor.h"
#include "JzRE/RHI/JzRenderBackend.h"
#include "JzRE/RHI/JzRenderFrontend.h"

namespace JzRE {

class JzREInstance {
public:
  JzREInstance(JzERHIType rhiType, std::filesystem::path &openDirectory);
  ~JzREInstance();

  void Run();

private:
  JzRE::Bool IsRunning() const;

  JzRef<JzEditor> m_editor;

  JzRenderFrontend m_renderFrontend;
  JzRef<JzRenderBackend> m_renderBackend;
};

} // namespace JzRE