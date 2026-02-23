/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#pragma once

#include <memory>

#include "JzRE/Runtime/Core/JzRETypes.h"

namespace JzRE {

class JzGPUResource;

/**
 * @brief Resource type used in RHI barrier commands.
 */
enum class JzEResourceType : U8 {
    Texture,
    Buffer
};

/**
 * @brief Logical resource state used by RHI barrier commands.
 */
enum class JzERHIResourceState : U8 {
    Unknown,
    Read,
    Write,
    ReadWrite
};

/**
 * @brief One resource transition item recorded into command lists.
 */
struct JzRHIResourceBarrier {
    JzEResourceType                type;
    std::shared_ptr<JzGPUResource> resource;
    JzERHIResourceState            before = JzERHIResourceState::Unknown;
    JzERHIResourceState            after  = JzERHIResourceState::Unknown;
};

} // namespace JzRE
