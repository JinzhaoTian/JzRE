/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Resource/JzShaderProgram.h"

namespace JzRE {

JzShaderProgram::JzShaderProgram(const String &name) :
    m_name(name)
{
}

void JzShaderProgram::AddKeyword(const JzShaderKeyword &keyword)
{
    m_keywords.push_back(keyword);
}

I32 JzShaderProgram::GetKeywordIndex(const String &name) const
{
    for (const auto &keyword : m_keywords) {
        if (keyword.name == name) {
            return static_cast<I32>(keyword.index);
        }
    }
    return -1;
}

JzShaderVariantKey JzShaderProgram::GetDefaultVariantKey() const
{
    JzShaderVariantKeyBuilder builder;
    for (const auto &keyword : m_keywords) {
        if (keyword.defaultEnabled) {
            builder.EnableKeyword(keyword.index);
        }
    }
    return builder.Build();
}

} // namespace JzRE
