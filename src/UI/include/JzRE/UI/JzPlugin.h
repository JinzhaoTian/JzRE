/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/UI/JzEPlugin.h"

namespace JzRE {
/**
 * @brief Base class for all plugins
 */
class JzPlugin {
public:
    /**
     * @brief Destructor
     */
    virtual ~JzPlugin() = default;

    /**
     * @brief Execute the plugin behaviour
     *
     * @param context The execution context
     */
    virtual void Execute(JzEPluginExecutionContext context) = 0;

    /**
     * @brief User data
     */
    void *userData = nullptr;
};

} // namespace JzRE
