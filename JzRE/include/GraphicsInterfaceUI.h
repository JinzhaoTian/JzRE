#pragma once

#include "CommonTypes.h"

namespace JzRE {
class GraphicsInterfaceUI {
public:
    GraphicsInterfaceUI(RawPtr<GLFWwindow> window);
    ~GraphicsInterfaceUI();

    void Render();
};
} // namespace JzRE