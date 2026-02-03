/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <memory>
#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Editor/Panels/JzCanvas.h"
#include "JzRE/Editor/UI/JzPanel.h"
#include "JzRE/Editor/UI/JzPanelWindow.h"
#include "JzRE/Editor/Panels/JzMenuBar.h"

namespace JzRE {
/**
 * @brief Panels Manager of JzRE
 */
class JzPanelsManager {
public:
    /**
     * @brief Constructor
     *
     * @param canvas
     */
    JzPanelsManager(JzCanvas &canvas) :
        m_canvas(canvas) { }

    /**
     * @brief Destructor
     */
    ~JzPanelsManager() = default;

    /**
     * @brief Create a panel
     *
     * @tparam T The type of the panel
     * @tparam Args The types of the arguments
     * @param id The ID of the panel
     * @param args The arguments
     */
    template <typename T, typename... Args>
    void CreatePanel(const String &id, Args &&...args)
    {
        if constexpr (std::is_base_of<JzPanelWindow, T>::value) {
            m_panels.emplace(id, std::make_unique<T>(id, std::forward<Args>(args)...));
            T &instance = *static_cast<T *>(m_panels.at(id).get());
            GetPanelAs<JzMenuBar>("Menu Bar").RegisterPanel(instance.name, instance);
        } else {
            m_panels.emplace(id, std::make_unique<T>(std::forward<Args>(args)...));
        }

        m_canvas.AddPanel(*m_panels.at(id));
    }

    /**
     * @brief Get a panel as a specific type
     *
     * @tparam T The type of the panel
     * @param id The ID of the panel
     * @return The panel
     */
    template <typename T>
    T &GetPanelAs(const String &id)
    {
        return *static_cast<T *>(m_panels[id].get());
    }

private:
    std::unordered_map<String, std::unique_ptr<JzPanel>> m_panels;
    JzCanvas                                            &m_canvas;
};
} // namespace JzRE
