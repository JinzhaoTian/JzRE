#pragma once

#include "JzRETypes.h"
#include "JzDataWidget.h"
#include "JzEPlugin.h"
#include "JzEvent.h"
#include "JzWidgetContainer.h"

namespace JzRE {
/**
 * @brief Tree Node Widget
 */
class JzTreeNode : public JzDataWidget<JzRE::String>, public JzWidgetContainer {
public:
    /**
     * @brief Constructor
     *
     * @param p_name The name of the tree node
     * @param arrowClickToOpen Whether to open the tree node when the arrow is clicked
     */
    JzTreeNode(const String &p_name = "", Bool arrowClickToOpen = false);

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