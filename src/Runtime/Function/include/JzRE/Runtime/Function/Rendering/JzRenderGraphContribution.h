/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <functional>

#include "JzRE/Runtime/Core/JzMatrix.h"
#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Core/JzVector.h"
#include "JzRE/Runtime/Function/ECS/JzEntity.h"
#include "JzRE/Runtime/Function/Rendering/JzRenderTarget.h"
#include "JzRE/Runtime/Function/Rendering/JzRenderVisibility.h"

namespace JzRE {

struct JzRGPassContext;
class JzRHICommandList;
class JzWorld;

/**
 * @brief Per-pass execution context for render graph contributions.
 */
struct JzRenderGraphContributionContext {
    JzWorld               &world;
    JzEntity               camera;
    JzRenderVisibility     visibility;
    JzRenderTargetFeatures targetFeatures;
    JzIVec2                targetSize;
    const JzMat4          &viewMatrix;
    const JzMat4          &projectionMatrix;
    JzRHICommandList      *commandList = nullptr;
    const JzRGPassContext *passContext = nullptr;
};

/**
 * @brief Render target scope mask for contribution placement.
 */
enum class JzRenderGraphContributionScope : U8 {
    None             = 0,
    MainScene        = 1 << 0,
    RegisteredTarget = 1 << 1,
    All              = MainScene | RegisteredTarget,
};

inline constexpr JzRenderGraphContributionScope operator|(JzRenderGraphContributionScope lhs,
                                                          JzRenderGraphContributionScope rhs)
{
    return static_cast<JzRenderGraphContributionScope>(static_cast<U8>(lhs) | static_cast<U8>(rhs));
}

inline constexpr JzRenderGraphContributionScope operator&(JzRenderGraphContributionScope lhs,
                                                          JzRenderGraphContributionScope rhs)
{
    return static_cast<JzRenderGraphContributionScope>(static_cast<U8>(lhs) & static_cast<U8>(rhs));
}

inline constexpr Bool HasContributionScope(JzRenderGraphContributionScope mask,
                                           JzRenderGraphContributionScope scope)
{
    return static_cast<U8>(mask & scope) != 0;
}

/**
 * @brief A graph contribution that can append target-scoped render behavior.
 *
 * This abstraction allows runtime/editor features to contribute rendering work
 * without coupling to RenderSystem internals.
 */
struct JzRenderGraphContribution {
    String                                                        name;
    JzRenderTargetFeatures                                        requiredFeature = JzRenderTargetFeatures::None;
    JzRenderGraphContributionScope                                scope           = JzRenderGraphContributionScope::RegisteredTarget;
    Bool                                                          clearTarget     = false;
    std::function<Bool()>                                         enabledExecute  = nullptr;
    std::function<void(const JzRenderGraphContributionContext &)> execute;
};

} // namespace JzRE
