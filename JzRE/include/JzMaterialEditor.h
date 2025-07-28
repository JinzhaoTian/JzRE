#pragma once

#include "CommonTypes.h"
#include "JzPanelWindow.h"

namespace JzRE {
/**
 * @brief Material Editor
 */
class JzMaterialEditor : public JzPanelWindow {
public:
    /**
     * @brief Constructor
     *
     * @param name
     * @param is_opened
     */
    JzMaterialEditor(const String &name, Bool is_opened);
};
} // namespace JzRE