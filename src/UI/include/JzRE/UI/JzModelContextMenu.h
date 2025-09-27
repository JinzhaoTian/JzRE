/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/UI/JzFileContextMenu.h"

namespace JzRE {

/**
 * @brief Model Context menu
 */
class JzModelContextMenu : public JzFileContextMenu {
public:
    JzModelContextMenu(const String &filePath);

    virtual void CreateList() override;
};

} // namespace JzRE