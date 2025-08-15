#pragma once

#include "CommonTypes.h"
#include "JzWidget.h"
#include "JzWidgetContainer.h"

namespace JzRE {
/**
 * @brief Columns Widget
 */
template <Size N>
class JzColumns : public JzWidget, public JzWidgetContainer {
public:
    /**
     * @brief Constructor
     *
     * @param label The label of the button
     * @param p_disabled Whether the button is disabled
     */
    JzColumns()
    {
        widths.fill(-1.f);
    }

protected:
    /**
     * @brief Implementation of the Draw method
     */
    void _Draw_Impl() override
    {
        ImGui::Columns(static_cast<I32>(N), ("##" + m_widgetID).c_str(), false);

        I32 counter = 0;

        CollectGarbages();

        for (auto it = m_widgets.begin(); it != m_widgets.end();) {
            it->first->Draw();

            ++it;

            if (it != m_widgets.end()) {
                if (widths[counter] != -1.f)
                    ImGui::SetColumnWidth(counter, widths[counter]);

                ImGui::NextColumn();
            }

            ++counter;

            if (counter == N)
                counter = 0;
        }

        ImGui::Columns(1);
    }

public:
    std::array<F32, N> widths;
};

} // namespace JzRE