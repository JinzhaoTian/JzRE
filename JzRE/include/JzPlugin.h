#pragma once

namespace JzRE {
/**
 * @brief Plugin execution context
 */
enum class EPluginExecutionContext {
    /**
     * @brief Widget context
     */
    WIDGET,
    /**
     * @brief Panel context
     */
    PANEL
};

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
    virtual void Execute(EPluginExecutionContext context) = 0;

    /**
     * @brief User data
     */
    void *userData = nullptr;
};

} // namespace JzRE
