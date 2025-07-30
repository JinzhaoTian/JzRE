#include "JzConverter.h"

ImVec2 JzRE::JzConverter::ToImVec2(const JzRE::JzVec2 &value)
{
    return ImVec2(value.x(), value.y());
}

JzRE::JzVec2 JzRE::JzConverter::ToJzVec2(const ImVec2 &value)
{
    return JzRE::JzVec2(value.x, value.y);
}