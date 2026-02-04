/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <functional>
#include <utility>
#include <vector>

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Core/JzVector.h"
#include "JzRE/Runtime/Function/ECS/JzEntity.h"

namespace JzRE {

// Forward declaration
class JzRenderTarget;

/**
 * @brief Entry for a registered render target.
 *
 * Contains all information needed by RenderSystem to render to this target:
 * - target: The render target to render to
 * - camera: The camera entity to use for rendering
 * - includeEditor: Whether to include JzEditorOnlyTag entities
 * - includePreview: Whether to include JzPreviewOnlyTag entities
 * - shouldRender: Callback to check if this target should be rendered this frame
 * - getDesiredSize: Callback to get the desired size (for dynamic resizing)
 * - name: Debug name for logging
 */
struct JzRenderTargetEntry {
    JzRenderTarget          *target         = nullptr;
    JzEntity                 camera         = INVALID_ENTITY;
    Bool                     includeEditor  = false;
    Bool                     includePreview = false;
    std::function<Bool()>    shouldRender;
    std::function<JzIVec2()> getDesiredSize;
    String                   name;
};

/**
 * @brief Registry for render targets.
 *
 * Views register their render targets here during initialization.
 * RenderSystem iterates all registered targets and renders them.
 *
 * This provides a unified rendering path where all views are rendered
 * by the RenderSystem during World::Update(), rather than each View
 * calling RenderToTarget() individually during the UI phase.
 */
class JzRenderTargetRegistry {
public:
    using Handle                           = U32;
    static constexpr Handle INVALID_HANDLE = 0;

    /**
     * @brief Register a render target entry.
     *
     * @param entry The entry to register
     * @return Handle to the registered entry
     */
    Handle Register(JzRenderTargetEntry entry);

    /**
     * @brief Unregister a render target.
     *
     * @param handle Handle returned from Register()
     */
    void Unregister(Handle handle);

    /**
     * @brief Update the camera for a registered target.
     *
     * @param handle Handle returned from Register()
     * @param camera New camera entity
     */
    void UpdateCamera(Handle handle, JzEntity camera);

    /**
     * @brief Get all registered entries.
     *
     * @return Vector of (handle, entry) pairs
     */
    std::vector<std::pair<Handle, JzRenderTargetEntry>> &GetEntries();

    /**
     * @brief Get all registered entries (const).
     *
     * @return Vector of (handle, entry) pairs
     */
    const std::vector<std::pair<Handle, JzRenderTargetEntry>> &GetEntries() const;

private:
    std::vector<std::pair<Handle, JzRenderTargetEntry>> m_entries;
    Handle                                              m_nextHandle = 1;
};

} // namespace JzRE
