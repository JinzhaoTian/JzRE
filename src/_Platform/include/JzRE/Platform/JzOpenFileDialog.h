/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Core/JzRETypes.h"
#include "JzRE/Platform/JzFileDialog.h"

namespace JzRE {
/**
 * @brief Open File Dialog
 */
class JzOpenFileDialog : public JzFileDialog {
public:
    /**
     * @brief Constructor
     *
     * @param p_dialogTitle Title of the dialog window
     */
    JzOpenFileDialog(const String &p_dialogTitle);

    /**
     * @brief Add a supported file type to the dialog window
     *
     * @param p_label
     * @param p_filter
     */
    void AddFileType(const String &p_label, const String &p_filter);

    /**
     * @brief Show Dialog
     */
    virtual void Show(JzEFileDialogType type) override;
};
} // namespace JzRE