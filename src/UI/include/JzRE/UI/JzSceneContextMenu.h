#pragma once

#include "JzRE/UI/JzFileContextMenu.h"

namespace JzRE {

/**
 * @brief Scene Context Menu
 */
class JzSceneContextMenu : public JzFileContextMenu {
public:
    /**
     * @brief Constructor
     *
     * @param filePath
     */
    JzSceneContextMenu(const String &filePath);

    /**
     * @brief Create an item list
     */
    virtual void CreateList() override;
};

} // namespace JzRE