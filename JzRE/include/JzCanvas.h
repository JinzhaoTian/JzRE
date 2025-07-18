#pragma once

#include "CommonTypes.h"
#include "IJzDrawable.h"
#include "JzPanel.h"

namespace JzRE {
/**
 * @brief Canvas class
 */
class JzCanvas : public IJzDrawable {
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
    Bool                                  m_isDockspace;
    List<std::reference_wrapper<JzPanel>> m_panels;
};
} // namespace JzRE
