/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <functional>
#include <memory>

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Core/JzVector.h"
#include "JzRE/Runtime/Function/ECS/JzEntity.h"
#include "JzRE/Runtime/Function/Rendering/JzRenderOutput.h"
#include "JzRE/Runtime/Function/Rendering/JzRenderVisibility.h"

namespace JzRE {

/**
 * @brief Handle type for registered render targets.
 */
using JzRenderTargetHandle = U32;

/**
 * @brief Invalid render target handle value.
 */
inline constexpr JzRenderTargetHandle INVALID_RENDER_TARGET_HANDLE = 0;

/**
 * @brief Per-target render feature flags.
 *
 * A render target can opt in to additional render features
 * (such as skybox and axis overlays) without forcing those features
 * into all render targets.
 */
enum class JzRenderTargetFeatures : U32 {
    None        = 0,
    Skybox      = 1 << 0,
    Axis        = 1 << 1,
    Grid        = 1 << 2,
    Manipulator = 1 << 3,
};

/**
 * @brief Bitwise OR for JzRenderTargetFeatures.
 */
inline constexpr JzRenderTargetFeatures operator|(JzRenderTargetFeatures lhs,
                                                  JzRenderTargetFeatures rhs)
{
    return static_cast<JzRenderTargetFeatures>(static_cast<U32>(lhs) | static_cast<U32>(rhs));
}

/**
 * @brief Bitwise AND for JzRenderTargetFeatures.
 */
inline constexpr JzRenderTargetFeatures operator&(JzRenderTargetFeatures lhs,
                                                  JzRenderTargetFeatures rhs)
{
    return static_cast<JzRenderTargetFeatures>(static_cast<U32>(lhs) & static_cast<U32>(rhs));
}

/**
 * @brief Check if feature mask contains a specific feature.
 */
inline constexpr Bool HasFeature(JzRenderTargetFeatures mask, JzRenderTargetFeatures feature)
{
    return static_cast<U32>(mask & feature) != 0;
}

/**
 * @brief Logical render target descriptor.
 */
struct JzRenderTargetDesc {
    String                   name;
    JzEntity                 camera     = INVALID_ENTITY;
    JzRenderVisibility       visibility = JzRenderVisibility::MainScene;
    JzRenderTargetFeatures   features   = JzRenderTargetFeatures::None;
    std::function<Bool()>    shouldRender;
    std::function<JzIVec2()> getDesiredSize;
};

/**
 * @brief Runtime render target instance.
 */
struct JzRenderTarget {
    JzRenderTargetHandle            handle = INVALID_RENDER_TARGET_HANDLE;
    JzRenderTargetDesc              desc;
    std::shared_ptr<JzRenderOutput> output;
};

} // namespace JzRE
