#pragma once

#include "CommonTypes.h"
#include "JzPanelWindow.h"

namespace JzRE {
class JzHierarchy : public JzPanelWindow {
public:
    JzHierarchy(const String &name, Bool is_opened);
};
} // namespace JzRE