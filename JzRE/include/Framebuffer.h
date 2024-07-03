#pragma once

#include "CommonTypes.h"
#include "Color.h"

namespace JzRE {

class Framebuffer {
public:
    Framebuffer() = delete;
    ~Framebuffer();
    Framebuffer(I32 w, I32 h);

    void Clear();

    void Resize(I32 w, I32 h);
    I32 GetWidth() const;
    I32 GetHeight() const;

    void SetColorPixel(I32 x, I32 y, const Color &c);
    // void GetColorPixel(I32 x, I32 y, F32& r, F32& g, F32& b, F32& a) const;

    void SetDepthPixel(I32 x, I32 y, F32 depth);
    // void GetDepthPixel(I32 x, I32 y, F32& depth) const;

public:
    U32 *data = nullptr;
    I32 width, height;
};

} // namespace JzRE