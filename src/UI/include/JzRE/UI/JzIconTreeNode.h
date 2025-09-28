/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Core/JzRETypes.h"
#include "JzRE/Core/JzEvent.h"
#include "JzRE/RHI/JzRHITexture.h"
#include "JzRE/UI/JzDataWidget.h"
#include "JzRE/UI/JzWidgetContainer.h"

namespace JzRE {

/**
 * @brief Icon tree node
 */
class JzIconTreeNode : public JzDataWidget<String>, public JzWidgetContainer {
public:
    /**
     * @brief Constructor
     *
     * @param name
     * @param iconTexture
     * @param arrowClickToOpen
     */
    JzIconTreeNode(const String &name, std::shared_ptr<JzRHITexture> iconTexture, Bool arrowClickToOpen = false);

    /**
     * @brief Open the tree node
     */
    void Open();

    /**
     * @brief Close the tree node
     */
    void Close();

    /**
     * @brief Check if the tree node is opened
     *
     * @return True if the tree node is opened, false otherwise
     */
    Bool IsOpened() const;

protected:
    /**
     * @brief Draw Implement
     */
    virtual void _Draw_Impl() override;

public:
    String    name;
    Bool      selected = false;
    Bool      leaf     = false;
    JzEvent<> ClickedEvent;
    JzEvent<> DoubleClickedEvent;
    JzEvent<> OpenedEvent;
    JzEvent<> ClosedEvent;

private:
    std::shared_ptr<JzRHITexture> m_iconTexture;
    Bool                          m_arrowClickToOpen = false;
    Bool                          m_shouldOpen       = false;
    Bool                          m_shouldClose      = false;
    Bool                          m_opened           = false;
};

} // namespace JzRE