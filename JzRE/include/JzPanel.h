#pragma once

#include "CommonTypes.h"
#include "IJzDrawable.h"
#include "JzWidgetContainer.h"

namespace JzRE {
class JzPanel : public IJzDrawable, public JzWidgetContainer {
public:
    /**
     * @brief Constructor
     */
    JzPanel();

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
