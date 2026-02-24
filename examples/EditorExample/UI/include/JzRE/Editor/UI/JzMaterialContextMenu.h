/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#pragma once

#include "JzRE/Editor/UI/JzFileContextMenu.h"

namespace JzRE {

/**
 * @brief Material Context menu
 */
class JzMaterialContextMenu : public JzFileContextMenu {
public:
    /**
     * @brief Constructor
     *
     * @param filePath
     */
    JzMaterialContextMenu(const String &filePath);

    /**
     * @brief Create an item list
     */
    virtual void CreateList() override;
};

} // namespace JzRE
