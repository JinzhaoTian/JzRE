#pragma once

#include "CommonTypes.h"

namespace JzRE {

class Color {
public:
    F32 r, g, b, a;

public:
    Color();
    Color(F32 r, F32 g, F32 b, F32 a);
    Color(const Color &rhs);
    ~Color();

    Color operator+(const Color &right) const;
    Color operator+(const float c) {
        return Color(r + c, g + c, b + c, a);
    }
    Color operator-(const Color &right) const;
    Color operator*(const Color &right) const;
    Color operator*(F32 value) const;
    Color operator/(F32 value) const;

    static U32 Rgba(F32 r, F32 g, F32 b, F32 a);
    static U32 Rgba(const Color &color);

    // static Color Lerp(const Color& left, const Color& right, F32 t);
    static Color white;
};

} // namespace JzRE