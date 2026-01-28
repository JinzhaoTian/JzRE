/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#ifdef _WIN32

#include "JzRE/Runtime/Platform/Dialog/JzSaveFileDialog.h"

JzRE::JzSaveFileDialog::JzSaveFileDialog(const JzRE::String &dialogTitle) :
    JzFileDialog(dialogTitle) { }

void JzRE::JzSaveFileDialog::DefineExtension(const JzRE::String &label, const JzRE::String &extension)
{
    m_filter    = label + '\0' + '*' + extension + '\0';
    m_extension = extension;
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

#endif // _WIN32