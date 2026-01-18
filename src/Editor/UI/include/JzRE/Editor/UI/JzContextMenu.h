/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Editor/UI/JzPlugin.h"
#include "JzWidgetContainer.h"

namespace JzRE {

/**
 * @brief Context menu plugin
 */
class JzContextMenu : public JzPlugin, public JzWidgetContainer {
public:
    /**
     * @brief Execute the plugin
     *
     * @param context
     */
    void Execute(JzEPluginExecutionContext context) override;

    /**
     * @brief Close the Context menu
     */
    void Close();
};

}; // namespace JzRE