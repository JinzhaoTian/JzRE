#pragma once

#include "CommonTypes.h"
#include "JzEPlugin.h"
#include "JzPlugin.h"

namespace JzRE {
/**
 * @brief Plugin Data dispatcher class
 *
 * @tparam T
 */
template <typename T>
class JzDataDispatcher : public JzPlugin {
public:
    /**
     * @brief Register a reference to a variable
     *
     * @param reference
     */
    void RegisterReference(T &reference)
    {
        m_dataPointer = &reference;
    }

    /**
     * @brief Register a provider function
     *
     * @param provider
     */
    void RegisterProvider(std::function<void(T)> provider)
    {
        m_provider = provider;
    }

    /**
     * @brief Register a gatherer function
     *
     * @param gatherer
     */
    void RegisterGatherer(std::function<T(void)> gatherer)
    {
        m_gatherer = gatherer;
    }

    /**
     * @brief Provide data to the dispatcher
     *
     * @param data
     */
    void Provide(T data)
    {
        if (m_valueChanged) {
            if (m_dataPointer)
                *m_dataPointer = data;
            else
                m_provider(data);

            m_valueChanged = false;
        }
    }

    /**
     * @brief Notify the dispatcher that the data has changed
     */
    void NotifyChange()
    {
        m_valueChanged = true;
    }

    /**
     * @brief Gather data from the dispatcher
     *
     * @return T
     */
    T Gather()
    {
        return m_dataPointer ? *m_dataPointer : m_gatherer();
    }

    /**
     * @brief Execute the plugin
     *
     * @param p_context
     */
    virtual void Execute(JzEPluginExecutionContext p_context) override { }

private:
    Bool                   m_valueChanged = false;
    T                     *m_dataPointer  = nullptr;
    std::function<void(T)> m_provider;
    std::function<T(void)> m_gatherer;
};
} // namespace JzRE