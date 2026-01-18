/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Editor/UI/JzFileContextMenu.h"

namespace JzRE {

/**
 * @brief Shader Context menu
 */
class JzShaderContextMenu : public JzFileContextMenu {
public:
    /**
     * @brief Constructor
     *
     * @param filePath
     */
    JzShaderContextMenu(const String &filePath);

    /**
     * @brief Create an item list
     */
    virtual void CreateList() override;
};

} // namespace JzRE