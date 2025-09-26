/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

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

    /**
     * @brief Convert the given Hex to ImVec4
     *
     * @param hexColor
     * @param alpha
     *
     * @return ImVec4
     */
    static ImVec4 HexToImVec4(const String &hexColor, JzRE::F32 alpha = 1.0f);

    /**
     * @brief Convert the given ImVec4 color to Hex
     *
     * @param color
     * @param includeAlpha
     *
     * @return String
     */
    static String ImVec4ToImHex(const ImVec4 &color, Bool includeAlpha = false);

    /**
     * @brief Convert the given Hex to ImColor
     *
     * @param hexColor
     * @param alpha
     *
     * @return ImColor
     */
    static ImColor HexToImColor(const String &hexColor, float alpha = 1.0f);
};
} // namespace JzRE