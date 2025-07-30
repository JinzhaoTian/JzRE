#pragma once

#include "CommonTypes.h"
#include "JzVector.h"

namespace JzRE {
class JzConverter {
public:
    /**
     * Disabled constructor
     */
    JzConverter() = delete;

    /**
     * Convert the given FVector2 to ImVec2
     * @param p_value
     */
    static ImVec2 ToImVec2(const JzVec2 &value);

    /**
     * Convert the given ImVec2 to FVector2
     * @param p_value
     */
    static JzVec2 ToJzVec2(const ImVec2 &value);
};
} // namespace JzRE