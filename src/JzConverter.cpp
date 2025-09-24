/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzConverter.h"
#include <sstream>

ImVec2 JzRE::JzConverter::ToImVec2(const JzRE::JzVec2 &value)
{
    return ImVec2(value.x(), value.y());
}

JzRE::JzVec2 JzRE::JzConverter::ToJzVec2(const ImVec2 &value)
{
    return JzRE::JzVec2(value.x, value.y);
}

ImVec4 JzRE::JzConverter::ToImVec4(const JzRE::JzVec4 &value)
{
    return ImVec4(value.x(), value.y(), value.z(), value.w());
}

JzRE::JzVec4 JzRE::JzConverter::ToJzVec4(const ImVec4 &value)
{
    return JzRE::JzVec4(value.x, value.y, value.z, value.w);
}

ImColor JzRE::JzConverter::HexToImColor(const JzRE::String &hexColor, JzRE::F32 alpha)
{
    String hex = hexColor;
    if (hex[0] == '#') {
        hex.erase(0, 1);
    }

    U32               colorValue;
    std::stringstream ss;
    ss << std::hex << hex;
    ss >> colorValue;

    F32 r = ((colorValue >> 16) & 0xFF) / 255.0f;
    F32 g = ((colorValue >> 8) & 0xFF) / 255.0f;
    F32 b = (colorValue & 0xFF) / 255.0f;

    return ImColor(r / 255.0f, g / 255.0f, b / 255.0f, alpha);
}