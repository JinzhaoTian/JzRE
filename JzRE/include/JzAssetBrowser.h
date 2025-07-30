#pragma once

#include "CommonTypes.h"
#include "JzButton.h"
#include "JzGroup.h"
#include "JzPanelWindow.h"
#include "JzSeparator.h"

namespace JzRE {
/**
 * @brief Asset browser panel
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
    JzGroup *m_assetList;
};
} // namespace JzRE