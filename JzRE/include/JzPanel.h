#pragma once

#include "CommonTypes.h"
#include "JzIDrawable.h"
#include "JzWidgetContainer.h"

namespace JzRE {
class JzPanel : public JzIDrawable, public JzWidgetContainer {
public:
    /**
     * @brief Constructor
     *
     * @param panelId The ID of the panel
     */
    JzPanel(const String &panelId);

    /**
     * @brief Draw the panel
     */
    void Draw() override;

    /**
     * @brief Get the panel ID
     *
     * @return const String&
     */
    const String &GetPanelID() const;

public:
    Bool enabled = true;

protected:
    /**
     * @brief Draw the panel
     */
    virtual void _Draw_Impl() = 0;

    String m_panelId;

private:
    static U64 __PANEL_ID_INCREMENT;
};

} // namespace JzRE
