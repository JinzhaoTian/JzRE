/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#pragma once

#include <variant>

#include "JzRE/Runtime/Core/JzMatrix.h"
#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Core/JzVector.h"

namespace JzRE {

/**
 * @brief Unified shader parameter value used by all graphics backends.
 */
using JzShaderParameterValue = std::variant<I32, F32, JzVec2, JzVec3, JzVec4, JzMat3, JzMat4>;

/**
 * @brief Named shader parameter entry.
 */
struct JzShaderParameter {
    String                 name;
    JzShaderParameterValue value;
};

} // namespace JzRE
