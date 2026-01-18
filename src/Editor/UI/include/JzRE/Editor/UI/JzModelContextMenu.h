/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Editor/UI/JzFileContextMenu.h"

namespace JzRE {

/**
 * @brief Model Context menu
 */
class JzModelContextMenu : public JzFileContextMenu {
public:
    /**
     * @brief Constructor
     *
     * @param filePath
     */
    JzModelContextMenu(const String &filePath);

    /**
     * @brief Create an item list
     */
    virtual void CreateList() override;
};

} // namespace JzRE