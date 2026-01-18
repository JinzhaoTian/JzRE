/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <memory>
#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Resource/JzShaderVariantKey.h"

namespace JzRE {

class JzRHIPipeline;

/**
 * @brief Represents a compiled shader variant.
 *
 * A variant is a specific compilation of a shader program with
 * certain keywords enabled/disabled. Each variant holds a reference
 * to the compiled RHI pipeline.
 */
class JzShaderVariant {
public:
    /**
     * @brief Construct a shader variant.
     * @param key The variant key identifying which keywords are enabled.
     * @param pipeline The compiled RHI pipeline.
     */
    JzShaderVariant(JzShaderVariantKey key, std::shared_ptr<JzRHIPipeline> pipeline);

    /**
     * @brief Get the variant key.
     * @return The key identifying this variant.
     */
    const JzShaderVariantKey &GetKey() const
    {
        return m_key;
    }

    /**
     * @brief Get the RHI pipeline.
     * @return Shared pointer to the compiled pipeline.
     */
    std::shared_ptr<JzRHIPipeline> GetPipeline() const
    {
        return m_pipeline;
    }

    /**
     * @brief Check if this variant is valid and usable.
     * @return True if the pipeline is valid and linked.
     */
    Bool IsValid() const;

private:
    JzShaderVariantKey             m_key;
    std::shared_ptr<JzRHIPipeline> m_pipeline;
};

} // namespace JzRE
