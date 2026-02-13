/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Core/JzRETypes.h"

namespace JzRE {

/**
 * @brief Visibility mask for entity filtering during rendering.
 *
 * Used to control which entities are rendered by different render targets:
 * - MainScene: default gameplay entities (without channel tags)
 * - Overlay: auxiliary helpers and debug overlays
 * - Isolated: entities dedicated to isolated previews/sandbox outputs
 */
enum class JzRenderVisibility : U8 {
    None      = 0,
    Overlay   = 1 << 0, ///< Entities with JzOverlayRenderTag
    Isolated  = 1 << 1, ///< Entities with JzIsolatedRenderTag
    MainScene = 1 << 2, ///< Entities without explicit render-channel tags
    All       = Overlay | Isolated | MainScene,
};

/**
 * @brief Bitwise OR for JzRenderVisibility.
 */
inline constexpr JzRenderVisibility operator|(JzRenderVisibility lhs, JzRenderVisibility rhs)
{
    return static_cast<JzRenderVisibility>(static_cast<U8>(lhs) | static_cast<U8>(rhs));
}

/**
 * @brief Bitwise AND for JzRenderVisibility.
 */
inline constexpr JzRenderVisibility operator&(JzRenderVisibility lhs, JzRenderVisibility rhs)
{
    return static_cast<JzRenderVisibility>(static_cast<U8>(lhs) & static_cast<U8>(rhs));
}

/**
 * @brief Check if visibility mask contains a specific flag.
 */
inline constexpr Bool HasVisibility(JzRenderVisibility mask, JzRenderVisibility flag)
{
    return static_cast<U8>(mask & flag) != 0;
}

} // namespace JzRE
