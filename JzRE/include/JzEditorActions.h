#pragma once

#include "JzContext.h"
#include "JzPanelsManager.h"

namespace JzRE {
class JzEditorActions {
public:
    JzEditorActions(JzContext &context, JzPanelsManager &panelsManager);

private:
    JzContext       &m_context;
    JzPanelsManager &m_panelsManager;
};
} // namespace JzRE