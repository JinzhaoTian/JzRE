#pragma once

#include "JzEPlugin.h"
#include "JzPlugin.h"

#include <vector>

namespace JzRE {
/**
 * @brief Interface for Pluginable Objects
 */
class JzIPluginable {
public:
    /**
     * @brief Destructor
     */
    virtual ~JzIPluginable()
    {
        RemoveAllPlugins();
    }

    /**
     * @brief Add a plugin
     *
     * @tparam T The type of the plugin
     * @tparam Args The types of the arguments
     * @param p_args The arguments
     * @return The plugin
     */
    template <typename T, typename... Args>
    T &AddPlugin(Args &&...p_args)
    {
        static_assert(std::is_base_of<JzPlugin, T>::value, "T should derive from IPlugin");

        T *newPlugin = new T(std::forward<Args>(p_args)...);
        m_plugins.push_back(newPlugin);
        return *newPlugin;
    }

    /**
     * @brief Returns the plugin of the given type, or nullptr if not found
     *
     * @tparam T The type of the plugin
     * @return The plugin
     */
    template <typename T>
    T *GetPlugin()
    {
        static_assert(std::is_base_of<JzPlugin, T>::value, "T should derive from IPlugin");

        for (auto it = m_plugins.begin(); it != m_plugins.end(); ++it) {
            T *result = dynamic_cast<T *>(*it);
            if (result)
                return result;
        }

        return nullptr;
    }

    /**
     * @brief Execute every plugins
     *
     * @param context The execution context
     */
    void ExecutePlugins(JzEPluginExecutionContext context)
    {
        for (auto &plugin : m_plugins)
            plugin->Execute(context);
    }

    /**
     * @brief Remove every plugins
     */
    void RemoveAllPlugins()
    {
        for (auto &plugin : m_plugins)
            delete plugin;

        m_plugins.clear();
    }

private:
    std::vector<JzPlugin *> m_plugins;
};

} // namespace JzRE