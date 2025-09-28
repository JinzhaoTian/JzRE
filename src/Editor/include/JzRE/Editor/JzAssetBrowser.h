/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <filesystem>
#include "JzRE/Core/JzRETypes.h"
#include "JzRE/UI/JzPanelWindow.h"
#include "JzRE/UI/JzGroup.h"
#include "JzRE/UI/JzTreeNode.h"

namespace JzRE {

/**
 * @brief Asset Browser Panel Window
 */
class JzAssetBrowser : public JzPanelWindow {
public:
    /**
     * @brief Constructor
     *
     * @param title The title of the asset browser panel
     */
    JzAssetBrowser(const String &name, Bool is_opened);

    /**
     * @brief Fill the asset browser panel
     */
    void Fill();

    /**
     * @brief Clear the asset browser panel
     */
    void Clear();

    /**
     * @brief Refresh the asset browser panel
     */
    void Refresh();

private:
    void _TraverseDirectory(JzTreeNode &root, const std::filesystem::path &path);
    void _AddDirectoryItem(JzTreeNode *root, const std::filesystem::path &path, Bool autoOpen);
    void _AddFileItem(JzTreeNode *root, const std::filesystem::path &path);

private:
    JzGroup              *m_assetList;
    std::filesystem::path m_openDirectory;
};

} // namespace JzRE