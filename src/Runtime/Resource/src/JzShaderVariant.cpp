/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Resource/JzShaderVariant.h"
#include "JzRE/Runtime/Platform/JzRHIPipeline.h"

namespace JzRE {

JzShaderVariant::JzShaderVariant(JzShaderVariantKey key, std::shared_ptr<JzRHIPipeline> pipeline) :
    m_key(key),
    m_pipeline(std::move(pipeline))
{
}

Bool JzShaderVariant::IsValid() const
{
    return m_pipeline != nullptr;
}

} // namespace JzRE
