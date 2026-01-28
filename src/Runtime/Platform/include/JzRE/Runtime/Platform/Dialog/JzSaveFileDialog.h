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
class JzSaveFileDialog : public JzFileDialog {
public:
    /**
     * @brief Constructor
     *
     * @param pdialogTitle Title of the dialog window
     */
    JzSaveFileDialog(const String &dialogTitle);

    /**
     * @brief Define the extension of the saved file
     *
     * @param label
     * @param extension
     */
    void DefineExtension(const String &label, const String &pextension);

    /**
     * @brief Show Dialog
     */
    virtual void Show(JzEFileDialogType type = JzEFileDialogType::SaveFile) override;

private:
    void _addExtensionToFilePathAndName();

private:
    String m_extension;
};
} // namespace JzRE