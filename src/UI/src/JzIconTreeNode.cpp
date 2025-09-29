/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/UI/JzIconTreeNode.h"
#include <imgui.h>

JzRE::JzIconTreeNode::JzIconTreeNode(const String &p_name, std::shared_ptr<JzRHITexture> iconTexture, Bool arrowClickToOpen) :
    JzDataWidget(name),
    name(p_name),
    m_iconTexture(iconTexture),
    m_arrowClickToOpen(arrowClickToOpen)
{
    m_autoExecutePlugins = false;
}

void JzRE::JzIconTreeNode::Open()
{
    m_shouldOpen  = true;
    m_shouldClose = false;
}

void JzRE::JzIconTreeNode::Close()
{
    m_shouldClose = true;
    m_shouldOpen  = false;
}

JzRE::Bool JzRE::JzIconTreeNode::IsOpened() const
{
    return m_opened;
}

void JzRE::JzIconTreeNode::_Draw_Impl()
{
    Bool prevOpened = m_opened;

    if (m_shouldOpen) {
        ImGui::SetNextItemOpen(true);
        m_shouldOpen = false;
    } else if (m_shouldClose) {
        ImGui::SetNextItemOpen(false);
        m_shouldClose = false;
    }

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None;
    if (m_arrowClickToOpen) flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    if (selected) flags |= ImGuiTreeNodeFlags_Selected;
    if (leaf) flags |= ImGuiTreeNodeFlags_Leaf;

    Bool opened = ImGui::TreeNodeEx((name + m_widgetID).c_str(), flags);

    if (ImGui::IsItemClicked() && (ImGui::GetMousePos().x - ImGui::GetItemRectMin().x) > ImGui::GetTreeNodeToLabelSpacing()) {
        ClickedEvent.Invoke();

        if (ImGui::IsMouseDoubleClicked(0)) {
            DoubleClickedEvent.Invoke();
        }
    }

    if (opened) {
        if (!prevOpened)
            OpenedEvent.Invoke();

        m_opened = true;

        ExecutePlugins(JzEPluginExecutionContext::WIDGET); // Manually execute plugins to make plugins considering the TreeNode and no childs

        DrawWidgets();

        ImGui::TreePop();
    } else {
        if (prevOpened)
            ClosedEvent.Invoke();

        m_opened = false;

        ExecutePlugins(JzEPluginExecutionContext::WIDGET); // Manually execute plugins to make plugins considering the TreeNode and no childs
    }
}