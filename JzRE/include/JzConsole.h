#pragma once

#include "CommonTypes.h"
#include "JzPanelWindow.h"

namespace JzRE {
/**
 * @brief Console
 */
class JzConsole : public JzPanelWindow {
public:
    /**
     * @brief Constructor
     *
     * @param name
     * @param is_opened
     */
    JzConsole(const String &name, Bool is_opened);
};
} // namespace JzRE