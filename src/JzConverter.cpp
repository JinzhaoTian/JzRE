#include "JzConverter.h"

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