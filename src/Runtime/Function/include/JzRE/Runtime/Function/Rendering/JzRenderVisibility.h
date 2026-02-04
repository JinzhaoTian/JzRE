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
 * Used to control which entities are rendered in different views:
 * - GameView: Untagged only (game objects)
 * - SceneView: Untagged | EditorOnly (game objects + editor helpers)
 * - AssetView: PreviewOnly (preview entities only)
 */
enum class JzRenderVisibility : U8 {
    None        = 0,
    EditorOnly  = 1 << 0, ///< Entities with JzEditorOnlyTag (grid, gizmo, etc.)
    PreviewOnly = 1 << 1, ///< Entities with JzPreviewOnlyTag (asset preview)
    Untagged    = 1 << 2, ///< Regular game entities (no special tag)
    All         = EditorOnly | PreviewOnly | Untagged,
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
