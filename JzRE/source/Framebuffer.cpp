#include "Framebuffer.h"

namespace JzRE {

Framebuffer::Framebuffer(I32 w, I32 h) :
    width(w), height(h) {
    this->data = new U32[width * height];
    memset(this->data, 0, width * height * sizeof(U32));
}

Framebuffer::~Framebuffer() {
    delete[] this->data;
}

void Framebuffer::Resize(I32 w, I32 h) {
    U32 *newData = new U32[w * h];

    for (int i = 0; i < (std::min)(height, h); ++i) {
        for (int j = 0; j < (std::min)(width, w); ++j) {
            newData[i * w + j] = this->data[i * width + j];
        }
    }

    delete[] this->data;

    this->data = newData;
    this->width = w;
    this->height = h;
}

I32 Framebuffer::GetWidth() const {
    return width;
}

I32 Framebuffer::GetHeight() const {
    return height;
}

void Framebuffer::SetColorPixel(I32 x, I32 y, const Color &c) {
    int index = y * width + x;
    this->data[index] = Color::Rgba(c);
}

void Framebuffer::SetDepthPixel(I32 x, I32 y, F32 depth) {
    int index = y * width + x;
    this->data[index] = depth;
}

void Framebuffer::Clear() {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            ((DWORD *)this->data)[y * width + x] = RGB(0, 0, 0);
        }
    }
}

} // namespace JzRE