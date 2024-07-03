#include "Renderer.h"
#include "Matrix.h"

namespace JzRE {

void Renderer::DrawPixel(I32 x, I32 y, const Color &color) {
    framebuffer->SetColorPixel(x, y, color);
}

void Renderer::DrawLine(I32 x1, I32 y1, I32 x2, I32 y2, const Color &c) {
    I32 x, y, rem = 0;
    if (x1 == x2 && y1 == y2) {
        DrawPixel(x1, y1, c);
    } else if (x1 == x2) {
        I32 inc = (y1 <= y2) ? 1 : -1;
        for (y = y1; y != y2; y += inc) DrawPixel(x1, y, c);
        DrawPixel(x2, y2, c);
    } else if (y1 == y2) {
        int inc = (x1 <= x2) ? 1 : -1;
        for (x = x1; x != x2; x += inc) DrawPixel(x, y1, c);
        DrawPixel(x2, y2, c);
    } else {
        int dx = (x1 < x2) ? x2 - x1 : x1 - x2;
        int dy = (y1 < y2) ? y2 - y1 : y1 - y2;
        if (dx >= dy) {
            if (x2 < x1) x = x1, y = y1, x1 = x2, y1 = y2, x2 = x, y2 = y;
            for (x = x1, y = y1; x <= x2; x++) {
                DrawPixel(x, y, c);
                rem += dy;
                if (rem >= dx) {
                    rem -= dx;
                    y += (y2 >= y1) ? 1 : -1;
                    DrawPixel(x, y, c);
                }
            }
            DrawPixel(x2, y2, c);
        } else {
            if (y2 < y1) x = x1, y = y1, x1 = x2, y1 = y2, x2 = x, y2 = y;
            for (x = x1, y = y1; y <= y2; y++) {
                DrawPixel(x, y, c);
                rem += dx;
                if (rem >= dy) {
                    rem -= dy;
                    x += (x2 >= x1) ? 1 : -1;
                    DrawPixel(x, y, c);
                }
            }
            DrawPixel(x2, y2, c);
        }
    }
}

void Renderer::DrawPrimitive(Vertex v0, Vertex v1, Vertex v2) {
}

} // namespace JzRE