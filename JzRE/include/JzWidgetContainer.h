#pragma once

#include "CommonTypes.h"
#include "JzEMemoryMode.h"
#include "JzWidget.h"

namespace JzRE {
/**
 * @brief Widgets Container
 */
class JzWidgetContainer {
public:
    /**
     * @brief Constructor
     */
    JzWidgetContainer() = default;

    /**
     * @brief Destructor
     */
    virtual ~JzWidgetContainer();

    /**
     * @brief Remove a widget from the container
     *
     * @param p_widget The widget to remove
     */
    void RemoveWidget(JzWidget &p_widget);

    /**
     * @brief Remove all widgets from the container
     */
    void RemoveAllWidgets();

    /**
     * @brief Consider a widget
     *
     * @param p_widget The widget to consider
     * @param p_manageMemory The memory management mode
     */
    void ConsiderWidget(JzWidget &widget, Bool manageMemory = true);

    /**
     * @brief Unconsider a widget
     *
     * @param p_widget The widget to unconsider
     */
    void UnconsiderWidget(JzWidget &p_widget);

    /**
     * @brief Collect garbages by removing widgets marked as "Destroyed"
     */
    void CollectGarbages();

    /**
     * @brief Draw every widgets
     */
    void DrawWidgets();

    /**
     * @brief Allow the user to reverse the draw order of this widget container
     *
     * @param reversed The reversed state
     */
    void ReverseDrawOrder(Bool reversed = true);

    /**
     * @brief Create a widget
     *
     * @tparam T The type of the widget
     * @tparam Args The types of the arguments
     * @param p_args The arguments
     * @return The widget
     */
    template <typename T, typename... Args>
    T &CreateWidget(Args &&...p_args)
    {
        m_widgets.emplace_back(new T(p_args...), JzEMemoryMode::INTERNAL_MANAGMENT);
        T &instance = *reinterpret_cast<T *>(m_widgets.back().first);
        instance.SetParent(this);
        return instance;
    }

    /**
     * @brief Returns the widgets and their memory management mode
     *
     * @return The widgets and their memory management mode
     */
    std::vector<std::pair<JzWidget *, JzEMemoryMode>> &GetWidgets();

protected:
    std::vector<std::pair<JzWidget *, JzEMemoryMode>> m_widgets;
    Bool                                              m_reverseDrawOrder = false;
};

} // namespace JzRE