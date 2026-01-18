/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Resource/JzShaderVariantKey.h"

namespace JzRE {

JzShaderVariantKeyBuilder &JzShaderVariantKeyBuilder::EnableKeyword(U32 keywordIndex)
{
    if (keywordIndex < 64) {
        m_mask |= (1ULL << keywordIndex);
    }
    return *this;
}

JzShaderVariantKeyBuilder &JzShaderVariantKeyBuilder::DisableKeyword(U32 keywordIndex)
{
    if (keywordIndex < 64) {
        m_mask &= ~(1ULL << keywordIndex);
    }
    return *this;
}

JzShaderVariantKeyBuilder &JzShaderVariantKeyBuilder::SetKeyword(U32 keywordIndex, Bool enabled)
{
    if (enabled) {
        return EnableKeyword(keywordIndex);
    } else {
        return DisableKeyword(keywordIndex);
    }
}

JzShaderVariantKey JzShaderVariantKeyBuilder::Build() const
{
    JzShaderVariantKey key;
    key.keywordMask = m_mask;
    return key;
}

JzShaderVariantKeyBuilder &JzShaderVariantKeyBuilder::Reset()
{
    m_mask = 0;
    return *this;
}

} // namespace JzRE
