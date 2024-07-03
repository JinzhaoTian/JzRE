#pragma once

#include "CommonTypes.h"
#include "Framebuffer.h"

namespace JzRE {

class GraphicsInterfaceRenderWindow {
public:
public:
    GraphicsInterfaceRenderWindow(I32 w, I32 h, const String &title);
    ~GraphicsInterfaceRenderWindow();
};

} // namespace JzRE