#pragma once

#include "CommonTypes.h"
#include "JzDataWidget.h"
#include "JzEPlugin.h"
#include "JzEvent.h"
#include "JzWidgetContainer.h"

namespace JzRE {
class JzTreeNode : public JzDataWidget<JzRE::String>, public JzWidgetContainer {
public:
    JzTreeNode(const String &p_name = "", Bool arrowClickToOpen = false);

    void Open();

    void Close();

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