/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzConverter.h"
#include <iomanip>
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

ImVec4 JzRE::JzConverter::HexToImVec4(const JzRE::String &hexColor, JzRE::F32 alpha)
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

    return ImVec4(r, g, b, alpha);
}

JzRE::String JzRE::JzConverter::ImVec4ToImHex(const ImVec4 &color, JzRE::Bool includeAlpha)
{
    U32 r = static_cast<U32>(color.x * 255.0f + 0.5f);
    U32 g = static_cast<U32>(color.y * 255.0f + 0.5f);
    U32 b = static_cast<U32>(color.z * 255.0f + 0.5f);

    std::stringstream ss;
    ss << "#";
    ss << std::hex << std::setw(2) << std::setfill('0') << r;
    ss << std::hex << std::setw(2) << std::setfill('0') << g;
    ss << std::hex << std::setw(2) << std::setfill('0') << b;

    if (includeAlpha) {
        U32 a = static_cast<U32>(color.w * 255.0f + 0.5f);
        ss << std::hex << std::setw(2) << std::setfill('0') << a;
    }

    return ss.str();
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

    return ImColor(r, g, b, alpha);
}