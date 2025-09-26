/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Core/JzRETypes.h"

namespace JzRE {

/**
 * @brief Represents an entity in the ECS. It's just an identifier.
 */
using JzEntity                = U32;
const JzEntity INVALID_ENTITY = -1;

} // namespace JzRE