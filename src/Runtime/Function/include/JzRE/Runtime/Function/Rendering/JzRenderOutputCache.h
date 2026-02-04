/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <unordered_map>
#include <memory>

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Function/Rendering/JzRenderOutput.h"

namespace JzRE {

/**
 * @brief Stable output cache for UI consumption.
 *
 * Keeps outputs alive across frames to provide stable handles.
 */
class JzRenderOutputCache {
public:
    /**
     * @brief Get or create a cached output for name.
     *
     * @param name Output name
     * @param output Latest output instance
     * @return Cached output pointer
     */
    JzRenderOutput *GetOrCreate(const String &name, std::shared_ptr<JzRenderOutput> output);

    /**
     * @brief Get cached output by name.
     *
     * @param name Output name
     * @return Cached output pointer or nullptr
     */
    JzRenderOutput *Get(const String &name) const;

    /**
     * @brief Update cached output instance.
     *
     * @param name Output name
     * @param output Latest output instance
     */
    void Update(const String &name, std::shared_ptr<JzRenderOutput> output);

    /**
     * @brief Remove a cached output.
     *
     * @param name Output name
     */
    void Remove(const String &name);

    /**
     * @brief Clear all cached outputs.
     */
    void Clear();

private:
    std::unordered_map<String, std::shared_ptr<JzRenderOutput>> m_outputs;
};

} // namespace JzRE
