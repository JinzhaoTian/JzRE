/**
 * @file    JzShaderAssetFactory.h
 * @brief   Factory for creating JzShaderAsset resources
 * @author  Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <filesystem>
#include "JzRE/Runtime/Resource/JzResourceFactory.h"
#include "JzRE/Runtime/Resource/JzShaderAsset.h"

namespace JzRE {

/**
 * @brief Factory for creating JzShaderAsset resources.
 *
 * This factory supports two modes of shader loading:
 * 1. Single path: Expects .vert and .frag files with the same base name
 * 2. Pipe-separated paths: "vertex.vert|fragment.frag"
 *
 * @example
 * @code
 * // Mode 1: Base name
 * factory.Create("shaders/standard");
 * // Creates shader from shaders/standard.vert and shaders/standard.frag
 *
 * // Mode 2: Explicit paths
 * factory.Create("shaders/custom.vert|shaders/custom.frag");
 * @endcode
 */
class JzShaderAssetFactory : public JzResourceFactory {
public:
    /**
     * @brief Create a shader asset resource.
     *
     * The name parameter can be:
     * - A base path (e.g., "shaders/standard") which expects .vert and .frag extensions
     * - A pipe-separated pair (e.g., "vertex.vert|fragment.frag")
     * - A .vert or .frag file path (will look for matching counterpart)
     *
     * @param name The shader path specification.
     *
     * @return JzResource* Pointer to the created JzShaderAsset.
     */
    virtual JzResource *Create(const String &name) override
    {
        namespace fs = std::filesystem;

        // Check if name contains pipe separator (explicit vert|frag)
        size_t pipePos = name.find('|');
        if (pipePos != String::npos) {
            String vertexPath   = name.substr(0, pipePos);
            String fragmentPath = name.substr(pipePos + 1);
            return new JzShaderAsset(vertexPath, fragmentPath);
        }

        // Check file extension
        fs::path filePath(name);
        String   extension = filePath.extension().string();

        if (extension == ".vert") {
            // Look for matching fragment shader
            fs::path fragmentPath = filePath;
            fragmentPath.replace_extension(".frag");
            return new JzShaderAsset(name, fragmentPath.string());
        }

        if (extension == ".frag") {
            // Look for matching vertex shader
            fs::path vertexPath = filePath;
            vertexPath.replace_extension(".vert");
            return new JzShaderAsset(vertexPath.string(), name);
        }

        // No extension or unrecognized - treat as base name
        return new JzShaderAsset(name);
    }
};

} // namespace JzRE
