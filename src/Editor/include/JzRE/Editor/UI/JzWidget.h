/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Editor/UI/JzIDrawable.h"
#include "JzRE/Editor/UI/JzIPluginable.h"

namespace JzRE {

class JzWidgetContainer;

/**
 * @brief Base class for all widgets
 */
class JzWidget : public JzIDrawable, public JzIPluginable {
public:
    /**
     * @brief Constructor
     */
    JzWidget();

    /**
     * @brief Draw the widget on the panel
     */
    virtual void Draw() override;

    /**
     * @brief Link the widget to another widget
     *
     * @param widget
     */
    void LinkTo(const JzWidget &widget);

    /**
     * @brief Destroy the widget
     */
    void Destroy();

    /**
     * @brief Check if the widget is destroyed
     *
     * @return Bool
     */
    Bool IsDestroyed() const;

    /**
     * @brief Set the parent of the widget
     *
     * @param parent
     */
    void SetParent(JzWidgetContainer *parent);

    /**
     * @brief Check if the widget has a parent
     *
     * @return Bool
     */
    Bool HasParent() const;

    /**
     * @brief Get the parent of the widget
     *
     * @return JzWidgetContainer*
     */
    JzWidgetContainer *GetParent();

protected:
    /**
     * @brief Implementation of the Draw method
     */
    virtual void _Draw_Impl() = 0;

public:
    String tooltip;
    Bool   enabled   = true;
    Bool   disabled  = false;
    Bool   lineBreak = true;

protected:
    JzWidgetContainer *m_parent;
    String             m_widgetID           = "?";
    Bool               m_autoExecutePlugins = true;

private:
    static U64 __WIDGET_ID_INCREMENT;
    Bool       m_destroyed = false;
};

} // namespace JzRE