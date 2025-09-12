/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRETypes.h"
#include "JzFileDialog.h"

namespace JzRE {
/**
 * @brief Open File Dialog
 */
class JzSaveFileDialog : public JzFileDialog {
public:
    /**
     * @brief Constructor
     *
     * @param p_dialogTitle Title of the dialog window
     */
    JzSaveFileDialog(const String &p_dialogTitle);

    /**
     * @brief Define the extension of the saved file
     *
     * @param p_label
     * @param p_extension
     */
    void DefineExtension(const String &p_label, const String &p_extension);

    /**
     * @brief Show Dialog
     */
    virtual void Show() override;

private:
    void _addExtensionToFilePathAndName();

private:
    String m_extension;
};
} // namespace JzRE