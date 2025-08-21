#pragma once

#include "CommonTypes.h"
#include "JzPanelWindow.h"
#include "JzImage.h"
#include "JzTextClickable.h"
#include "JzPathParser.h"
#include "JzGroup.h"
#include "JzTreeNode.h"

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
    void ParseFolder(JzTreeNode &p_root, const std::filesystem::directory_entry &p_directory, Bool p_isEngineItem, Bool p_scriptFolder = false);
    void ConsiderItem(JzTreeNode *p_root, const std::filesystem::directory_entry &p_entry, Bool p_isEngineItem, Bool p_autoOpen = false, Bool p_scriptFolder = false);

private:
    JzGroup              *m_assetList;
};

} // namespace JzRE