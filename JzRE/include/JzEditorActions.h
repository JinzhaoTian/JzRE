#pragma once

#include "JzContext.h"
#include "JzPanelsManager.h"
#include "JzServiceContainer.h"

#define EDITOR_EXEC(action) \
    JzRE::JzServiceContainer::Get<JzRE::JzEditorActions>().action
#define EDITOR_BIND(method, ...) \
    std::bind(&JzRE::JzEditorActions::method, &JzRE::JzServiceContainer::Get<JzRE::JzEditorActions>(), ##__VA_ARGS__)
#define EDITOR_EVENT(target) \
    JzRE::JzServiceContainer::Get<JzRE::JzEditorActions>().target
#define EDITOR_CONTEXT(instance) \
    JzRE::JzServiceContainer::Get<JzRE::JzEditorActions>().GetContext().instance
#define EDITOR_PANEL(type, id) \
    JzRE::JzServiceContainer::Get<JzRE::JzEditorActions>().GetPanelsManager().GetPanelAs<type>(id)

namespace JzRE {
/**
 * @brief Editor actions
 */
class JzEditorActions {
public:
    /**
     * @brief Constructor
     *
     * @param context
     * @param panelsManager
     */
    JzEditorActions(JzContext &context, JzPanelsManager &panelsManager);

    /**
     * @brief Destructor
     */
    virtual ~JzEditorActions();

    /**
     * @brief Get the context
     *
     * @return JzContext&
     */
    JzContext &GetContext();

    /**
     * @brief Get the panels manager
     *
     * @return JzPanelsManager&
     */
    JzPanelsManager &GetPanelsManager();

    /**
     * @brief Load an empty scene
     */
    void LoadEmptyScene();

private:
    JzContext       &m_context;
    JzPanelsManager &m_panelsManager;
};
} // namespace JzRE