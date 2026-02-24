/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <filesystem>

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Function/Project/JzProjectManager.h"
#include "JzRE/Runtime/Function/Asset/JzAssetImporter.h"
#include "JzRE/Runtime/Function/Asset/JzAssetExporter.h"

namespace JzRE {

/**
 * @brief Runtime-backed CLI command context.
 */
class JzCliContext {
public:
    /**
     * @brief Initialize service container and core services.
     *
     * @return Bool True when initialized.
     */
    Bool Initialize();

    /**
     * @brief Shutdown context resources.
     */
    void Shutdown();

    /**
     * @brief Load project into context.
     *
     * @param projectPath Path to project file.
     *
     * @return JzEProjectResult Load result.
     */
    JzEProjectResult LoadProject(const std::filesystem::path &projectPath);

    /**
     * @brief Access project manager.
     */
    JzProjectManager &GetProjectManager();

    /**
     * @brief Access asset importer.
     */
    JzAssetImporter &GetAssetImporter();

    /**
     * @brief Access asset exporter.
     */
    JzAssetExporter &GetAssetExporter();

private:
    Bool             m_initialized{false};
    JzProjectManager m_projectManager;
    JzAssetImporter  m_assetImporter;
    JzAssetExporter  m_assetExporter;
};

} // namespace JzRE
