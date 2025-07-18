#pragma once

#include "CommonTypes.h"
#include "JzPlugin.h"

namespace JzRE {

class IJzPluginable {
public:
    /**
     * Destructor
     */
    virtual ~IJzPluginable()
    {
        RemoveAllPlugins();
    }

    /**
     * Add a plugin
     * @param p_args
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
     * Returns the plugin of the given type, or nullptr if not found
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
     * Execute every plugins
     */
    void ExecutePlugins()
    {
        for (auto &plugin : m_plugins)
            plugin->Execute();
    }

    /**
     * Remove every plugins
     */
    void RemoveAllPlugins()
    {
        for (auto &plugin : m_plugins)
            delete plugin;

        m_plugins.clear();
    }

private:
    List<RawPtr<JzPlugin>> m_plugins;
};

} // namespace JzRE