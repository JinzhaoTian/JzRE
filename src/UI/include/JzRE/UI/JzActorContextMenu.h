#pragma once

#include "JzRE/UI/JzContextMenu.h"

namespace JzRE {

/**
 * @brief Actor context menu
 */
class JzActorContextMenu : public JzContextMenu {
public:
    /**
     * @brief Constructor
     */
    JzActorContextMenu();

    /**
     * @brief Execute the plugin
     *
     * @param context
     */
    void Execute(JzEPluginExecutionContext context) override;
};

} // namespace JzRE