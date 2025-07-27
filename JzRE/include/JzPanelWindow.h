#pragma once

#include "JzEvent.h"
#include "JzPanelTransformable.h"

namespace JzRE {
/**
 * @brief Panel Window
 */
class JzPanelWindow : public JzPanelTransformable {
public:
    /**
     * @brief Constructor
     *
     * @param panelId The ID of the panel window
     */
    JzPanelWindow(const String &name = "", Bool is_opened = true);

    /**
     * @brief Open the panel window
     */
    void Open();

    /**
     * @brief Close the panel window
     */
    void Close();

    /**
     * @brief Focus the panel window
     */
    void Focus();

    /**
     * @brief Set the opened state of the panel window
     */
    void SetOpened(Bool value);

    /**
     * @brief Check if the panel window is opened
     *
     * @return True if the panel window is opened, false otherwise
     */
    Bool IsOpened() const;

    /**
     * @brief Check if the panel window is hovered
     *
     * @return True if the panel window is hovered, false otherwise
     */
    Bool IsHovered() const;

    /**
     * @brief Check if the panel window is focused
     *
     * @return True if the panel window is focused, false otherwise
     */
    Bool IsFocused() const;

    /**
     * @brief Check if the panel window is appearing
     *
     * @return True if the panel window is appearing, false otherwise
     */
    Bool IsAppearing() const;

    /**
     * @brief Check if the panel window is visible
     *
     * @return True if the panel window is visible, false otherwise
     */
    Bool IsVisible() const;

    /**
     * @brief Scroll to the bottom of the panel window
     */
    void ScrollToBottom();

    /**
     * @brief Scroll to the top of the panel window
     */
    void ScrollToTop();

    /**
     * @brief Check if the panel window is scrolled to the bottom
     *
     * @return True if the panel window is scrolled to the bottom, false otherwise
     */
    Bool IsScrolledToBottom() const;

    /**
     * @brief Check if the panel window is scrolled to the top
     *
     * @return True if the panel window is scrolled to the top, false otherwise
     */
    Bool IsScrolledToTop() const;

protected:
    /**
     * @brief Implementation of the Draw method
     */
    void _Draw_Impl() override;

public:
    String name;

    Bool resizable                = true;
    Bool closable                 = false;
    Bool movable                  = true;
    Bool scrollable               = true;
    Bool dockable                 = false;
    Bool hideBackground           = false;
    Bool forceHorizontalScrollbar = false;
    Bool forceVerticalScrollbar   = false;
    Bool allowHorizontalScrollbar = false;
    Bool bringToFrontOnFocus      = true;
    Bool collapsable              = false;
    Bool allowInputs              = true;
    Bool titleBar                 = true;

    JzEvent<> OpenEvent;
    JzEvent<> CloseEvent;

private:
    Bool m_opened             = false;
    Bool m_hovered            = false;
    Bool m_focused            = false;
    Bool m_mustScrollToBottom = false;
    Bool m_mustScrollToTop    = false;
    Bool m_scrolledToBottom   = false;
    Bool m_scrolledToTop      = false;
};

} // namespace JzRE