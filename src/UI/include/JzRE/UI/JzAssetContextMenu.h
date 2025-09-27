/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <filesystem>
#include "JzRE/Core/JzEvent.h"
#include "JzRE/UI/JzContextMenu.h"

namespace JzRE {

/**
 * @brief Item Context menu
 */
class JzAssetContextMenu : public JzContextMenu {
public:
    /**
     * @brief Constructor
     *
     * @param filePath
     */
    JzAssetContextMenu(const String filePath);

    /**
     * @brief Create an item list
     */
    virtual void CreateList();

    /**
     * @brief Delete an item in list
     */
    virtual void DeleteItem() = 0;

    /**
     * @brief Execute the plugin
     *
     * @param context
     */
    void Execute(JzEPluginExecutionContext context) override;

public:
    std::filesystem::path                                 filePath;
    JzEvent<std::filesystem::path>                        DestroyedEvent;
    JzEvent<std::filesystem::path, std::filesystem::path> RenamedEvent;
};

}; // namespace JzRE