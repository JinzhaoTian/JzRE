#pragma once

#include "CommonTypes.h"
#include "JzPanelWindow.h"

namespace JzRE {
/**
 * @brief Base class for all view panels
 */
class JzView : public JzPanelWindow {
public:
    /**
     * @brief Constructor
     *
     * @param name
     * @param is_opened
     */
    JzView(const String &name, Bool is_opened);

protected:
    /**
     * @brief Implementation of the Draw method
     */
    void _Draw_Impl() override;
};

} // namespace JzRE