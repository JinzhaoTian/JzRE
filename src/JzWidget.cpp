#include "JzWidget.h"
#include <imgui.h>

uint64_t JzRE::JzWidget::__WIDGET_ID_INCREMENT = 0;

JzRE::JzWidget::JzWidget()
{
    m_widgetID = "##" + std::to_string(__WIDGET_ID_INCREMENT++);
}

void JzRE::JzWidget::Draw()
{
    if (enabled) {
        if (disabled) {
            ImGui::BeginDisabled();
        }

        _Draw_Impl();

        if (!tooltip.empty()) {
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip("%s", tooltip.c_str());
            }
        }

        if (disabled) {
            ImGui::EndDisabled();
        }

        if (m_autoExecutePlugins)
            ExecutePlugins(JzEPluginExecutionContext::WIDGET);

        if (!lineBreak) {
            ImGui::SameLine();
        }
    }
}

void JzRE::JzWidget::LinkTo(const JzRE::JzWidget &widget)
{
    m_widgetID = widget.m_widgetID;
}

void JzRE::JzWidget::Destroy()
{
    m_destroyed = true;
}

JzRE::Bool JzRE::JzWidget::IsDestroyed() const
{
    return m_destroyed;
}

void JzRE::JzWidget::SetParent(JzRE::JzWidgetContainer *parent)
{
    m_parent = parent;
}

JzRE::Bool JzRE::JzWidget::JzWidget::HasParent() const
{
    return m_parent;
}

JzRE::JzWidgetContainer *JzRE::JzWidget::JzWidget::GetParent()
{
    return m_parent;
}