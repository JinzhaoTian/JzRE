/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Editor/UI/JzIDrawable.h"
#include "JzRE/Editor/UI/JzPanel.h"

namespace JzRE {
/**
 * @brief Canvas
 */
class JzCanvas : public JzIDrawable {
public:
    /**
     * @brief Draw the canvas
     */
    void Draw() override;

    /**
     * @brief Set the dockspace state
     */
    void SetDockspace(Bool state);

    /**
     * @brief Check if the dockspace is enabled
     */
    Bool IsDockspace() const;

    /**
     * @brief Add a panel to the canvas
     */
    void AddPanel(JzPanel &panel);

    /**
     * @brief Remove a panel from the canvas
     */
    void RemovePanel(JzPanel &panel);

    /**
     * @brief Remove all panels from the canvas
     */
    void RemoveAllPanels();

private:
    Bool                                         m_isDockspace;
    std::vector<std::reference_wrapper<JzPanel>> m_panels;
};
} // namespace JzRE
