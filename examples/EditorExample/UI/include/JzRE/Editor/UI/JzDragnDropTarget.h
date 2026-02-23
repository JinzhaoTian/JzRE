/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <imgui.h>
#include <imgui_internal.h>
#include "JzRE/Editor/Core/JzEvent.h"
#include "JzRE/Editor/UI/JzPlugin.h"

namespace JzRE {

/**
 * @brief Represents a drag and drop target
 */
template <typename T>
class JzDragnDropTarget : public JzPlugin {
public:
    /**
     * @brief Constructor
     *
     * @param identifier
     * @param tooltip
     * @param data
     */
    JzDragnDropTarget(const String &identifier) :
        identifier(identifier) { }

    /**
     * @brief Execute the plugin behaviour
     *
     * @param context
     */
    virtual void Execute(JzEPluginExecutionContext context) override
    {
        const Bool result =
            context == JzEPluginExecutionContext::WIDGET ?
                ImGui::BeginDragDropTarget() :
                ImGui::BeginDragDropTargetCustom(
                    ImGui::GetCurrentWindow()->WorkRect,
                    ImGui::GetID(identifier.c_str()));

        if (result) {
            if (!m_isHovered)
                HoverStartEvent.Invoke();

            m_isHovered = true;

            ImGuiDragDropFlags target_flags = 0;
            // target_flags |= ImGuiDragDropFlags_AcceptBeforeDelivery;    // Don't wait until the delivery (release mouse button on a target) to do something

            if (!showYellowRect)
                target_flags |= ImGuiDragDropFlags_AcceptNoDrawDefaultRect; // Don't display the yellow rectangle

            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload(identifier.c_str(), target_flags)) {
                T data = *(T *)payload->Data;
                DataReceivedEvent.Invoke(data);
            }
            ImGui::EndDragDropTarget();
        } else {
            if (m_isHovered)
                HoverEndEvent.Invoke();

            m_isHovered = false;
        }
    }

    /**
     * @brief Is hovered
     *
     * @return Bool
     */
    Bool IsHovered() const
    {
        return m_isHovered;
    }

public:
    JzEvent<T> DataReceivedEvent;
    JzEvent<>  HoverStartEvent;
    JzEvent<>  HoverEndEvent;

public:
    String identifier;
    Bool   showYellowRect = true;

private:
    Bool m_isHovered;
};

} // namespace JzRE
