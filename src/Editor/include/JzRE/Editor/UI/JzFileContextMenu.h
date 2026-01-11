/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Editor/UI/JzAssetContextMenu.h"

namespace JzRE {

/**
 * @brief File Context menu
 */
class JzFileContextMenu : public JzAssetContextMenu {
public:
    /**
     * @brief Constructor
     *
     * @param filePath
     */
    JzFileContextMenu(const String &filePath);

    /**
     * @brief Create an item list
     */
    virtual void CreateList() override;

    /**
     * @brief Delete an item in list
     */
    virtual void DeleteItem() override;

public:
    JzEvent<std::filesystem::path> DuplicateEvent;
};

} // namespace JzRE