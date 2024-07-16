#pragma once

#include "CommonTypes.h"

namespace JzRE {
class GraphicsInterfaceTexture {
public:
    GraphicsInterfaceTexture();
    ~GraphicsInterfaceTexture();

    Bool LoadFromFile(const String &filepath);
    void Bind(U32 unit) const;

private:
    GLuint textureID;
};
} // namespace JzRE
