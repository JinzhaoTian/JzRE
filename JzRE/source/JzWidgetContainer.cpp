#include "JzWidgetContainer.h"

void JzRE::JzWidgetContainer::RemoveWidget(JzWidget &p_widget)
{
}

void JzRE::JzWidgetContainer::RemoveAllWidgets()
{
}

void JzRE::JzWidgetContainer::ConsiderWidget(JzWidget &p_widget, bool p_manageMemory)
{
}

void JzRE::JzWidgetContainer::UnconsiderWidget(JzWidget &p_widget)
{
}

void JzRE::JzWidgetContainer::CollectGarbages()
{
}

void JzRE::JzWidgetContainer::DrawWidgets()
{
}

void JzRE::JzWidgetContainer::ReverseDrawOrder(bool reversed)
{
}

std::vector<std::pair<JzRE::JzWidget *, JzRE::EMemoryMode>> &JzRE::JzWidgetContainer::GetWidgets()
{
    return m_widgets;
}
