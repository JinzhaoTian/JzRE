#pragma once

#include "CommonTypes.h"
#include "JzCanvas.h"
#include "JzMenuBar.h"
#include "JzPanel.h"
#include "JzPanelWindow.h"

namespace JzRE {
/**
 * @brief Panels manager class
 */
class JzPanelsManager {
public:
    /**
     * @brief Constructor
     *
     * @param canvas
     */
    JzPanelsManager(JzCanvas &canvas);

    /**
     * @brief Destructor
     */
    ~JzPanelsManager();

    /**
     * @brief Create a panel
     *
     * @tparam T
     * @param id
     * @param args
     */
    template <typename T, typename... Args>
    void CreatePanel(const String &id, Args &&...args)
    {
        if constexpr (std::is_base_of<JzPanelWindow, T>::value) {
            m_panels.emplace(id, MakeUnique<T>(id, std::forward<Args>(args)...));
            T &instance = *static_cast<T *>(m_panels.at(id).get());
            GetPanelAs<JzMenuBar>("Menu Bar").RegisterPanel(instance.name, instance);
        } else {
            m_panels.emplace(id, MakeUnique<T>(id, std::forward<Args>(args)...));
        }

        m_canvas.AddPanel(*m_panels.at(id));
    }

    /**
     * @brief Get a panel as a specific type
     *
     * @tparam T
     * @param p_id
     * @return T&
     */
    template <typename T>
    T &GetPanelAs(const String &id)
    {
        return *StaticCast<RawPtr<T>>(m_panels[id].get());
    }

private:
    UnorderedMap<String, UniquePtr<JzPanel>> m_panels;
    JzCanvas                                &m_canvas;
};
} // namespace JzRE
