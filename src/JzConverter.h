#pragma once

#include <imgui.h>
#include "JzVector.h"

namespace JzRE {
/**
 * @brief Converter Of Different Types
 */
class JzConverter {
public:
    /**
     * @brief Disabled constructor
     */
    JzConverter() = delete;

    /**
     * @brief Convert the given JzVec2 to ImVec2
     *
     * @param p_value
     */
    static ImVec2 ToImVec2(const JzVec2 &value);

    /**
     * @brief Convert the given ImVec2 to JzVec2
     *
     * @param p_value
     */
    static JzVec2 ToJzVec2(const ImVec2 &value);

    /**
     * @brief Convert the given JzVec4 to ImVec4
     *
     * @param p_value
     */
    static ImVec4 ToImVec4(const JzVec4 &value);

    /**
     * @brief Convert the given ImVec4 to JzVec4
     *
     * @param p_value
     */
    static JzVec4 ToJzVec4(const ImVec4 &value);
};
} // namespace JzRE