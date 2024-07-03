#include "Color.h"

namespace JzRE {

Color::Color() :
    r(0.0f), g(0.0f), b(0.0f), a(0.0f) {
}

Color::Color(F32 r, F32 g, F32 b, F32 a) :
    r(r), g(g), b(b), a(a) {
}

Color::Color(const Color &rhs) :
    r(rhs.r), g(rhs.g), b(rhs.b), a(rhs.a) {
}

Color::~Color() {
}

Color Color::operator+(const Color &right) const {
    return Color(r + right.r, g + right.g, b + right.b, a + right.a);
}

Color Color::operator-(const Color &right) const {
    return Color(r - right.r, g - right.g, b - right.b, a - right.a);
}

Color Color::operator*(const Color &right) const {
    return Color(r * right.r, g * right.g, b * right.b, a * right.a);
}

Color Color::operator*(F32 value) const {
    return Color(r * value, g * value, b * value, a * value);
}

Color Color::operator/(F32 value) const {
    F32 rhvalue = 1.0f / value;
    return Color(r * rhvalue, g * rhvalue, b * rhvalue, a * rhvalue);
}

// Color Color::Lerp(const Color& left, const Color& right, F32 t) {
//     return left + (right - left) * t;
// }
//
Color Color::white = Color(1, 1, 1, 1);

U32 Color::Rgba(F32 r, F32 g, F32 b, F32 a) {
    I32 R = (I32)(r * 255.0f);
    I32 G = (I32)(g * 255.0f);
    I32 B = (I32)(b * 255.0f);
    I32 A = (I32)(a * 255.0f);

    R = std::clamp(R, 0, 255);
    G = std::clamp(G, 0, 255);
    B = std::clamp(B, 0, 255);
    A = std::clamp(A, 0, 255);

    return RGBA(R, G, B, A);
}

U32 Color::Rgba(const Color &color) {
    I32 R = (I32)(color.r * 255.0f);
    I32 G = (I32)(color.g * 255.0f);
    I32 B = (I32)(color.b * 255.0f);
    I32 A = (I32)(color.a * 255.0f);

    R = std::clamp(R, 0, 255);
    G = std::clamp(G, 0, 255);
    B = std::clamp(B, 0, 255);
    A = std::clamp(A, 0, 255);

    return RGBA(R, G, B, A);
}

} // namespace JzRE