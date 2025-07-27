#pragma once

#include "CommonTypes.h"
#include "JzIDrawable.h"
#include "JzIPluginable.h"
#include "JzWidgetContainer.h"

namespace JzRE {
/**
 * @brief Base class for all panels
 */
class JzPanel : public JzIDrawable, public JzIPluginable, public JzWidgetContainer {
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
     * @brief Implementation of the Draw method
     */
    virtual void _Draw_Impl() = 0;

protected:
    String m_panelId;

private:
    static U64 __PANEL_ID_INCREMENT;
};

} // namespace JzRE
