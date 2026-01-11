/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Core/JzEvent.h"
#include "JzRE/Editor/UI/JzDataWidget.h"
#include "JzRE/Editor/UI/JzWidgetContainer.h"

namespace JzRE {

/**
 * @brief Tree Node Widget
 */
class JzTreeNode : public JzDataWidget<String>, public JzWidgetContainer {
public:
    /**
     * @brief Constructor
     *
     * @param name The name of the tree node
     * @param arrowClickToOpen Whether to open the tree node when the arrow is clicked
     */
    JzTreeNode(const String &name = "", Bool arrowClickToOpen = false);

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
    Bool m_arrowClickToOpen = false;
    Bool m_shouldOpen       = false;
    Bool m_shouldClose      = false;
    Bool m_opened           = false;
};

} // namespace JzRE