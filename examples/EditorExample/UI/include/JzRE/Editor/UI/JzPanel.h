/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Editor/UI/JzIDrawable.h"
#include "JzRE/Editor/UI/JzIPluginable.h"
#include "JzRE/Editor/UI/JzWidgetContainer.h"

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
     * @brief Draw Interface
     */
    virtual void _Draw_Impl() = 0;

protected:
    String m_panelId;

private:
    static U64 __PANEL_ID_INCREMENT;
};

} // namespace JzRE
