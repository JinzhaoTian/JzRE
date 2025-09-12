#include "JzWidgetContainer.h"
#include <algorithm>
#include <ranges>

JzRE::JzWidgetContainer::~JzWidgetContainer()
{
    RemoveAllWidgets();
}

void JzRE::JzWidgetContainer::RemoveWidget(JzWidget &p_widget)
{
    auto found = std::find_if(m_widgets.begin(), m_widgets.end(), [&p_widget](std::pair<JzRE::JzWidget *, JzRE::JzEMemoryMode> &p_pair) {
        return p_pair.first == &p_widget;
    });

    if (found != m_widgets.end()) {
        if (found->second == JzRE::JzEMemoryMode::INTERNAL_MANAGMENT)
            delete found->first;

        m_widgets.erase(found);
    }
}

void JzRE::JzWidgetContainer::RemoveAllWidgets()
{
    std::for_each(m_widgets.begin(), m_widgets.end(), [](auto &pair) {
        if (pair.second == JzRE::JzEMemoryMode::INTERNAL_MANAGMENT)
            delete pair.first;
    });

    m_widgets.clear();
}

void JzRE::JzWidgetContainer::ConsiderWidget(JzRE::JzWidget &widget, JzRE::Bool manageMemory)
{
    m_widgets.emplace_back(std::make_pair(&widget, manageMemory ? JzRE::JzEMemoryMode::INTERNAL_MANAGMENT : JzRE::JzEMemoryMode::EXTERNAL_MANAGMENT));
    widget.SetParent(this);
}

void JzRE::JzWidgetContainer::UnconsiderWidget(JzWidget &widget)
{
    auto found = std::find_if(m_widgets.begin(), m_widgets.end(), [&widget](std::pair<JzRE::JzWidget *, JzRE::JzEMemoryMode> &p_pair) {
        return p_pair.first == &widget;
    });

    if (found != m_widgets.end()) {
        widget.SetParent(nullptr);
        m_widgets.erase(found);
    }
}

void JzRE::JzWidgetContainer::CollectGarbages()
{
    m_widgets.erase(std::remove_if(m_widgets.begin(), m_widgets.end(), [](std::pair<JzRE::JzWidget *, JzRE::JzEMemoryMode> &p_item) {
                        bool toDestroy = p_item.first && p_item.first->IsDestroyed();

                        if (toDestroy && p_item.second == JzRE::JzEMemoryMode::INTERNAL_MANAGMENT)
                            delete p_item.first;

                        return toDestroy;
                    }),
                    m_widgets.end());
}

void JzRE::JzWidgetContainer::DrawWidgets()
{
    CollectGarbages();

    using WidgetType = decltype(m_widgets)::value_type::first_type;

    std::vector<WidgetType> widgetsToDraw;
    widgetsToDraw.reserve(m_widgets.size());
    std::ranges::copy(m_widgets | std::views::keys, std::back_inserter(widgetsToDraw));

    if (m_reverseDrawOrder) [[unlikely]] {
        for (WidgetType widget : widgetsToDraw | std::views::reverse) {
            widget->Draw();
        }
    } else {
        for (WidgetType widget : widgetsToDraw) {
            widget->Draw();
        }
    }
}

void JzRE::JzWidgetContainer::ReverseDrawOrder(JzRE::Bool reversed)
{
    m_reverseDrawOrder = reversed;
}

std::vector<std::pair<JzRE::JzWidget *, JzRE::JzEMemoryMode>> &JzRE::JzWidgetContainer::GetWidgets()
{
    return m_widgets;
}
