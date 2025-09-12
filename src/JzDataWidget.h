/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzDataDispatcher.h"
#include "JzWidget.h"

namespace JzRE {
/**
 * @brief Widget for displaying and editing data
 *
 * @tparam T The type of the data
 */
template <typename T>
class JzDataWidget : public JzWidget {
public:
    /**
     * @brief Construct a new JzDataWidget object
     *
     * @param data
     */
    JzDataWidget(T &data) :
        m_data(data) { };

    /**
     * @brief Draw the widget
     */
    virtual void Draw() override;

    /**
     * @brief Notify the widget that the data has changed
     */
    void NotifyChange();

private:
    T &m_data;
};

template <typename T>
inline void JzDataWidget<T>::Draw()
{
    if (enabled) {
        auto plugin = GetPlugin<JzRE::JzDataDispatcher<T>>();
        if (plugin) {
            m_data = plugin->Gather();
        }

        JzWidget::Draw();

        if (plugin) {
            plugin->Provide(m_data);
        }
    }
}

template <typename T>
inline void JzDataWidget<T>::NotifyChange()
{
    auto plugin = GetPlugin<JzRE::JzDataDispatcher<T>>();
    if (plugin) {
        plugin->NotifyChange();
    }
}
} // namespace JzRE