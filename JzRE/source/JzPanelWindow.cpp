#include "JzPanelWindow.h"

JzRE::JzPanelWindow::JzPanelWindow(const JzRE::String &name, JzRE::Bool is_opened) :
    name(name),
    m_opened(is_opened)
{
    autoSize = true;
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
    return false;
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

void JzRE::JzPanelWindow::_Draw_Impl()
{
    if (m_opened) {
        int windowFlags = ImGuiWindowFlags_None;

        if (!resizable) windowFlags |= ImGuiWindowFlags_NoResize;
        if (!movable) windowFlags |= ImGuiWindowFlags_NoMove;
        if (!dockable) windowFlags |= ImGuiWindowFlags_NoDocking;
        if (hideBackground) windowFlags |= ImGuiWindowFlags_NoBackground;
        if (forceHorizontalScrollbar) windowFlags |= ImGuiWindowFlags_AlwaysHorizontalScrollbar;
        if (forceVerticalScrollbar) windowFlags |= ImGuiWindowFlags_AlwaysVerticalScrollbar;
        if (allowHorizontalScrollbar) windowFlags |= ImGuiWindowFlags_HorizontalScrollbar;
        if (!bringToFrontOnFocus) windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
        if (!collapsable) windowFlags |= ImGuiWindowFlags_NoCollapse;
        if (!allowInputs) windowFlags |= ImGuiWindowFlags_NoInputs;
        if (!scrollable) windowFlags |= ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar;
        if (!titleBar) windowFlags |= ImGuiWindowFlags_NoTitleBar;

        ImVec2 minSizeConstraint /*= Internal::Converter::ToImVec2(minSize)*/;
        ImVec2 maxSizeConstraint /*= Internal::Converter::ToImVec2(maxSize)*/;

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

            ExecutePlugins(EPluginExecutionContext::PANEL);
            DrawWidgets();
        }

        ImGui::End();
    }
}