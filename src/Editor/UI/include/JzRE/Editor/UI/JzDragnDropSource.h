/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <imgui.h>
#include "JzRE/Runtime/Core/JzEvent.h"
#include "JzRE/Editor/UI/JzPlugin.h"

namespace JzRE {

/**
 * @brief Represents a drag and drop source
 */
template <typename T>
class JzDragnDropSource : public JzPlugin {
public:
    /**
     * @brief Constructor
     *
     * @param identifier
     * @param tooltip
     * @param data
     */
    JzDragnDropSource(const String &identifier, const String &tooltip, T data) :
        identifier(identifier),
        tooltip(tooltip),
        data(data) { }

    /**
     * @brief Execute the plugin behaviour
     *
     * @param context
     */
    virtual void Execute(JzEPluginExecutionContext context)
    {
        ImGuiDragDropFlags src_flags  = 0;
        src_flags                    |= ImGuiDragDropFlags_SourceNoDisableHover;     // Keep the source displayed as hovered
        src_flags                    |= ImGuiDragDropFlags_SourceNoHoldToOpenOthers; // Because our dragging is local, we disable the feature of opening foreign treenodes/tabs while dragging

        if (!hasTooltip)
            src_flags |= ImGuiDragDropFlags_SourceNoPreviewTooltip; // Hide the tooltip

        if (ImGui::BeginDragDropSource(src_flags)) {
            if (!m_isDragged)
                DragStartEvent.Invoke();

            m_isDragged = true;

            if (!(src_flags & ImGuiDragDropFlags_SourceNoPreviewTooltip))
                ImGui::Text(tooltip.c_str());
            ImGui::SetDragDropPayload(identifier.c_str(), &data, sizeof(data));
            ImGui::EndDragDropSource();
        } else {
            if (m_isDragged)
                DragStopEvent.Invoke();

            m_isDragged = false;
        }
    }

    /**
     * @brief Is dragged
     *
     * @return Bool
     */
    Bool IsDragged() const
    {
        return m_isDragged;
    }

public:
    JzEvent<> DragStartEvent;
    JzEvent<> DragStopEvent;

public:
    String identifier;
    Bool   hasTooltip = true;
    String tooltip;
    T      data;

private:
    Bool m_isDragged;
};

} // namespace JzRE