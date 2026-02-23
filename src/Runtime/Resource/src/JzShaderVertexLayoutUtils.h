/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#pragma once

#include <algorithm>
#include <optional>
#include <regex>
#include <vector>

#include "JzRE/Runtime/Platform/RHI/JzRHIPipeline.h"

namespace JzRE::JzShaderVertexLayoutUtils {

namespace {

struct JzParsedVertexAttribute {
    U32                      location = 0;
    JzEVertexAttributeFormat format   = JzEVertexAttributeFormat::Float3;
    U32                      size     = 0;
};

Bool ConvertGLSLTypeToVertexFormat(const String &glslType,
                                   JzEVertexAttributeFormat &outFormat,
                                   U32                      &outSize)
{
    if (glslType == "float") {
        outFormat = JzEVertexAttributeFormat::Float;
        outSize   = 4;
        return true;
    }
    if (glslType == "vec2") {
        outFormat = JzEVertexAttributeFormat::Float2;
        outSize   = 8;
        return true;
    }
    if (glslType == "vec3") {
        outFormat = JzEVertexAttributeFormat::Float3;
        outSize   = 12;
        return true;
    }
    if (glslType == "vec4") {
        outFormat = JzEVertexAttributeFormat::Float4;
        outSize   = 16;
        return true;
    }
    if (glslType == "int") {
        outFormat = JzEVertexAttributeFormat::Int;
        outSize   = 4;
        return true;
    }
    if (glslType == "ivec2") {
        outFormat = JzEVertexAttributeFormat::Int2;
        outSize   = 8;
        return true;
    }
    if (glslType == "ivec3") {
        outFormat = JzEVertexAttributeFormat::Int3;
        outSize   = 12;
        return true;
    }
    if (glslType == "ivec4") {
        outFormat = JzEVertexAttributeFormat::Int4;
        outSize   = 16;
        return true;
    }
    if (glslType == "uint") {
        outFormat = JzEVertexAttributeFormat::UInt;
        outSize   = 4;
        return true;
    }
    if (glslType == "uvec2") {
        outFormat = JzEVertexAttributeFormat::UInt2;
        outSize   = 8;
        return true;
    }
    if (glslType == "uvec3") {
        outFormat = JzEVertexAttributeFormat::UInt3;
        outSize   = 12;
        return true;
    }
    if (glslType == "uvec4") {
        outFormat = JzEVertexAttributeFormat::UInt4;
        outSize   = 16;
        return true;
    }

    return false;
}

} // namespace

/**
 * @brief Build a pipeline vertex layout from GLSL vertex input declarations.
 *
 * This parser looks for declarations in the form:
 * `layout(location = N) in <type> <name>;`
 *
 * @param vertexSource Vertex shader source code.
 *
 * @return Parsed vertex layout, or std::nullopt when no compatible declaration exists.
 */
inline std::optional<JzVertexLayoutDesc> BuildVertexLayoutFromVertexSource(const String &vertexSource)
{
    static const std::regex kInputRegex(
        R"(layout\s*\(\s*location\s*=\s*([0-9]+)\s*\)\s*in\s+([A-Za-z_][A-Za-z0-9_]*)\s+[A-Za-z_][A-Za-z0-9_]*\s*;)");

    std::vector<JzParsedVertexAttribute> parsedAttributes;

    for (std::sregex_iterator iter(vertexSource.begin(), vertexSource.end(), kInputRegex), end; iter != end; ++iter) {
        const auto &match = *iter;

        U32 location = 0;
        try {
            location = static_cast<U32>(std::stoul(match[1].str()));
        } catch (...) {
            continue;
        }

        JzEVertexAttributeFormat format = JzEVertexAttributeFormat::Float3;
        U32                      size   = 0;
        if (!ConvertGLSLTypeToVertexFormat(match[2].str(), format, size)) {
            continue;
        }

        const auto duplicate = std::find_if(
            parsedAttributes.begin(),
            parsedAttributes.end(),
            [location](const JzParsedVertexAttribute &attribute) {
                return attribute.location == location;
            });
        if (duplicate != parsedAttributes.end()) {
            continue;
        }

        parsedAttributes.push_back({location, format, size});
    }

    if (parsedAttributes.empty()) {
        return std::nullopt;
    }

    std::sort(parsedAttributes.begin(), parsedAttributes.end(), [](const JzParsedVertexAttribute &lhs, const JzParsedVertexAttribute &rhs) {
        return lhs.location < rhs.location;
    });

    JzVertexLayoutDesc layout;
    layout.bindings.push_back({0, 0, false});

    U32 currentOffset = 0;
    layout.attributes.reserve(parsedAttributes.size());
    for (const auto &attribute : parsedAttributes) {
        layout.attributes.push_back({attribute.location, 0, attribute.format, currentOffset});
        currentOffset += attribute.size;
    }

    if (currentOffset == 0) {
        return std::nullopt;
    }

    layout.bindings[0].stride = currentOffset;
    return layout;
}

} // namespace JzRE::JzShaderVertexLayoutUtils
