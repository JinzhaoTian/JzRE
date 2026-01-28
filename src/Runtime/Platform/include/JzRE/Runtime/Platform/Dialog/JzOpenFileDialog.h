/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Platform/Dialog/JzFileDialog.h"

namespace JzRE {
/**
 * @brief Open File Dialog
 */
class JzOpenFileDialog : public JzFileDialog {
public:
    /**
     * @brief Constructor
     *
     * @param dialogTitle Title of the dialog window
     */
    JzOpenFileDialog(const String &dialogTitle);

    /**
     * @brief Add a supported file type to the dialog window
     *
     * @param label
     * @param filter
     */
    void AddFileType(const String &label, const String &filter);

    /**
     * @brief Show Dialog
     */
    virtual void Show(JzEFileDialogType type) override;
};
} // namespace JzRE