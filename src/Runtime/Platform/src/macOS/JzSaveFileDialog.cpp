/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#ifdef __APPLE__

#include "JzRE/Runtime/Platform/Dialog/JzSaveFileDialog.h"

JzRE::JzSaveFileDialog::JzSaveFileDialog(const JzRE::String &p_dialogTitle) :
    JzFileDialog(p_dialogTitle) { }

void JzRE::JzSaveFileDialog::DefineExtension(const JzRE::String &p_label,
                                             const JzRE::String &p_extension)
{
    m_filter    = p_label + '\0' + '*' + p_extension + '\0';
    m_extension = p_extension;
}

void JzRE::JzSaveFileDialog::Show(JzEFileDialogType type)
{
    JzFileDialog::Show(type);

    if (m_succeeded)
        _addExtensionToFilePathAndName();
}

void JzRE::JzSaveFileDialog::_addExtensionToFilePathAndName()
{
    if (m_filepath.extension() != m_extension) {
        m_filepath.replace_extension(m_extension);
        m_filename = m_filepath.filename();
    }
}

#endif // __APPLE__
