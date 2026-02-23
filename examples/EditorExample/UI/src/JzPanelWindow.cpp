/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Editor/UI/JzPanelWindow.h"
#include "JzRE/Editor/UI/JzConverter.h"

JzRE::JzPanelWindow::JzPanelWindow(const JzRE::String &name, JzRE::Bool is_opened) :
    name(name),
    m_opened(is_opened)
{
    autoSize = false;
}

void JzRE::JzPanelWindow::Open()
{
    if (!m_opened) {
        m_opened = true;
        OpenEvent.Invoke();
    }
}

void JzRE::JzPanelWindow::Close()
{
    if (m_opened) {
        m_opened = false;
        CloseEvent.Invoke();
    }
}

void JzRE::JzPanelWindow::Focus()
{
    ImGui::SetWindowFocus((name + m_panelId).c_str());
}

void JzRE::JzPanelWindow::SetOpened(JzRE::Bool value)
{
    if (value != m_opened) {
        m_opened = value;

        if (m_opened)
            OpenEvent.Invoke();
        else
            CloseEvent.Invoke();
    }
}

JzRE::Bool JzRE::JzPanelWindow::IsOpened() const
{
    return m_opened;
}

JzRE::Bool JzRE::JzPanelWindow::IsHovered() const
{
    return m_hovered;
}

JzRE::Bool JzRE::JzPanelWindow::IsFocused() const
{
    return m_focused;
}

JzRE::Bool JzRE::JzPanelWindow::IsAppearing() const
{
    // TODO
    return false;
}

JzRE::Bool JzRE::JzPanelWindow::IsVisible() const
{
    // TODO
    return true;
}

void JzRE::JzPanelWindow::ScrollToBottom()
{
    m_mustScrollToBottom = true;
}

void JzRE::JzPanelWindow::ScrollToTop()
{
    m_mustScrollToTop = true;
}

JzRE::Bool JzRE::JzPanelWindow::IsScrolledToBottom() const
{
    return m_scrolledToBottom;
}

JzRE::Bool JzRE::JzPanelWindow::IsScrolledToTop() const
{
    return m_scrolledToTop;
}

void JzRE::JzPanelWindow::SetPosition(const JzRE::JzVec2 &position)
{
    m_position        = position;
    m_positionChanged = true;
}

void JzRE::JzPanelWindow::SetSize(const JzRE::JzVec2 &size)
{
    m_size        = size;
    m_sizeChanged = true;
}

void JzRE::JzPanelWindow::SetAlignment(JzRE::JzEHorizontalAlignment horizontalAlignment, JzRE::JzEVerticalAlignment verticalAlignment)
{
    m_horizontalAlignment = horizontalAlignment;
    m_verticalAlignment   = verticalAlignment;
    m_alignmentChanged    = true;
}

const JzRE::JzVec2 &JzRE::JzPanelWindow::GetPosition() const
{
    return m_position;
}

const JzRE::JzVec2 &JzRE::JzPanelWindow::GetSize() const
{
    return m_size;
}

JzRE::JzEHorizontalAlignment JzRE::JzPanelWindow::GetHorizontalAlignment() const
{
    return m_horizontalAlignment;
}

JzRE::JzEVerticalAlignment JzRE::JzPanelWindow::GetVerticalAlignment() const
{
    return m_verticalAlignment;
}

void JzRE::JzPanelWindow::Update()
{
    if (!m_firstFrame) {
        if (!autoSize)
            UpdateSize();
        CopyImGuiSize();

        UpdatePosition();
        CopyImGuiPosition();
    }

    m_firstFrame = false;
}

void JzRE::JzPanelWindow::_Draw_Impl()
{
    if (m_opened) {
        int windowFlags = ImGuiWindowFlags_None;

        if (!resizable)
            windowFlags |= ImGuiWindowFlags_NoResize;
        if (!movable)
            windowFlags |= ImGuiWindowFlags_NoMove;
        if (!dockable)
            windowFlags |= ImGuiWindowFlags_NoDocking;
        if (hideBackground)
            windowFlags |= ImGuiWindowFlags_NoBackground;
        if (forceHorizontalScrollbar)
            windowFlags |= ImGuiWindowFlags_AlwaysHorizontalScrollbar;
        if (forceVerticalScrollbar)
            windowFlags |= ImGuiWindowFlags_AlwaysVerticalScrollbar;
        if (allowHorizontalScrollbar)
            windowFlags |= ImGuiWindowFlags_HorizontalScrollbar;
        if (!bringToFrontOnFocus)
            windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
        if (!collapsable)
            windowFlags |= ImGuiWindowFlags_NoCollapse;
        if (!allowInputs)
            windowFlags |= ImGuiWindowFlags_NoInputs;
        if (!scrollable)
            windowFlags |= ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar;
        if (!titleBar)
            windowFlags |= ImGuiWindowFlags_NoTitleBar;

        ImVec2 minSizeConstraint = JzConverter::ToImVec2(minSize);
        ImVec2 maxSizeConstraint = JzConverter::ToImVec2(maxSize);

        /* Cancel constraint if x or y is <= 0.f */
        if (minSizeConstraint.x <= 0.f || minSizeConstraint.y <= 0.f)
            minSizeConstraint = {0.0f, 0.0f};

        if (maxSizeConstraint.x <= 0.f || maxSizeConstraint.y <= 0.f)
            maxSizeConstraint = {10000.f, 10000.f};

        ImGui::SetNextWindowSizeConstraints(minSizeConstraint, maxSizeConstraint);

        if (ImGui::Begin((name + m_panelId).c_str(), closable ? &m_opened : nullptr, windowFlags)) {
            m_hovered = ImGui::IsWindowHovered();
            m_focused = ImGui::IsWindowFocused();

            auto scrollY = ImGui::GetScrollY();

            m_scrolledToBottom = scrollY == ImGui::GetScrollMaxY();
            m_scrolledToTop    = scrollY == 0.0f;

            if (!m_opened)
                CloseEvent.Invoke();

            Update();

            if (m_mustScrollToBottom) {
                ImGui::SetScrollY(ImGui::GetScrollMaxY());
                m_mustScrollToBottom = false;
            }

            if (m_mustScrollToTop) {
                ImGui::SetScrollY(0.0f);
                m_mustScrollToTop = false;
            }

            ExecutePlugins(JzEPluginExecutionContext::PANEL);
            DrawWidgets();
        }

        ImGui::End();
    }
}

JzRE::JzVec2 JzRE::JzPanelWindow::CalculatePositionAlignmentOffset(JzRE::Bool p_default)
{
    JzVec2 result(0.0f, 0.0f);

    switch (p_default ? m_defaultHorizontalAlignment : m_horizontalAlignment) {
        case JzEHorizontalAlignment::LEFT:
            // No offset needed for left alignment
            break;
        case JzEHorizontalAlignment::CENTER:
            result.x -= m_size.x / 2.0f;
            break;
        case JzEHorizontalAlignment::RIGHT:
            result.x -= m_size.x;
            break;
    }

    switch (p_default ? m_defaultVerticalAlignment : m_verticalAlignment) {
        case JzEVerticalAlignment::TOP:
            // No offset needed for top alignment
            break;
        case JzEVerticalAlignment::MIDDLE:
            result.y -= m_size.y / 2.0f;
            break;
        case JzEVerticalAlignment::BOTTOM:
            result.y -= m_size.y;
            break;
    }

    return result;
}

void JzRE::JzPanelWindow::UpdatePosition()
{
    if (m_defaultPosition.x != -1.f && m_defaultPosition.y != 1.f) {
        JzVec2 offsettedDefaultPos = m_defaultPosition + CalculatePositionAlignmentOffset(true);
        ImGui::SetWindowPos(JzRE::JzConverter::ToImVec2(offsettedDefaultPos), ImGuiCond_Once);
    }

    if (m_positionChanged || m_alignmentChanged) {
        JzVec2 offset = CalculatePositionAlignmentOffset(false);
        JzVec2 offsetPos(m_position.x + offset.x, m_position.y + offset.y);
        ImGui::SetWindowPos(JzConverter::ToImVec2(offsetPos), ImGuiCond_Always);
        m_positionChanged  = false;
        m_alignmentChanged = false;
    }
}

void JzRE::JzPanelWindow::UpdateSize()
{
    if (m_sizeChanged) {
        ImGui::SetWindowSize(JzConverter::ToImVec2(m_size), ImGuiCond_Always);
        m_sizeChanged = false;
    }
}

void JzRE::JzPanelWindow::CopyImGuiPosition()
{
    m_position = JzConverter::ToJzVec2(ImGui::GetWindowPos());
}

void JzRE::JzPanelWindow::CopyImGuiSize()
{
    m_size = JzConverter::ToJzVec2(ImGui::GetWindowSize());
}