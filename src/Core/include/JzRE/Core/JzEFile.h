/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Core/JzRETypes.h"

namespace JzRE {
/**
 * @brief Enums of File Types
 */
enum class JzEFileType : U8 {
    UNKNOWN,
    MODEL,
    TEXTURE,
    SHADER,
    SHADER_PART,
    MATERIAL,
    SOUND,
    SCENE,
    SCRIPT,
    FONT
};

} // namespace JzRE