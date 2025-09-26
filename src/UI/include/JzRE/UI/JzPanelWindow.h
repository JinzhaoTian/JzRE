/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Core/JzEvent.h"
#include "JzRE/Core/JzVector.h"
#include "JzRE/UI/JzEAlignment.h"
#include "JzRE/UI/JzPanel.h"

namespace JzRE {
/**
 * @brief Panel Window
 */
class JzPanelWindow : public JzPanel {
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

    /**
     * @brief Set the position of the panel
     */
    void SetPosition(const JzVec2 &position);

    /**
     * @brief Set the size of the panel
     */
    void SetSize(const JzVec2 &size);

    /**
     * @brief Set the alignment of the panel
     */
    void SetAlignment(JzEHorizontalAlignment horizontalAlignment, JzEVerticalAlignment verticalAligment);

    /**
     * @brief Get the position of the panel
     */
    const JzVec2 &GetPosition() const;

    /**
     * @brief Get the size of the panel
     */
    const JzVec2 &GetSize() const;

    /**
     * @brief Get the horizontal alignment of the panel
     */
    JzEHorizontalAlignment GetHorizontalAlignment() const;

    /**
     * @brief Get the vertical alignment of the panel
     */
    JzEVerticalAlignment GetVerticalAlignment() const;

protected:
    /**
     * @brief Update the panel
     */
    void Update();

    /**
     * @brief Implementation of the Draw method
     */
    void _Draw_Impl() override;

private:
    JzVec2 CalculatePositionAlignmentOffset(Bool p_default = false);
    void   UpdatePosition();
    void   UpdateSize();
    void   CopyImGuiPosition();
    void   CopyImGuiSize();

public:
    String name;

    JzVec2 minSize = {0.f, 0.f};
    JzVec2 maxSize = {0.f, 0.f};

    Bool resizable                = true;
    Bool closable                 = true;
    Bool movable                  = true;
    Bool scrollable               = true;
    Bool dockable                 = true;
    Bool hideBackground           = false;
    Bool forceHorizontalScrollbar = false;
    Bool forceVerticalScrollbar   = false;
    Bool allowHorizontalScrollbar = false;
    Bool bringToFrontOnFocus      = true;
    Bool collapsable              = true;
    Bool allowInputs              = true;
    Bool titleBar                 = true;

    JzEvent<> OpenEvent;
    JzEvent<> CloseEvent;

    Bool autoSize = true;

protected:
    JzVec2                 m_defaultPosition            = {-1.f, -1.f};
    JzVec2                 m_defaultSize                = {-1.f, -1.f};
    JzEHorizontalAlignment m_defaultHorizontalAlignment = JzEHorizontalAlignment::LEFT;
    JzEVerticalAlignment   m_defaultVerticalAlignment   = JzEVerticalAlignment::TOP;
    JzVec2                 m_position                   = {0.0f, 0.0f};
    JzVec2                 m_size                       = {0.0f, 0.0f};
    Bool                   m_positionChanged            = false;
    Bool                   m_sizeChanged                = false;
    JzEHorizontalAlignment m_horizontalAlignment        = JzEHorizontalAlignment::LEFT;
    JzEVerticalAlignment   m_verticalAlignment          = JzEVerticalAlignment::TOP;
    Bool                   m_alignmentChanged           = false;
    Bool                   m_firstFrame                 = true;

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