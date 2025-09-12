/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzSaveFileDialog.h"

#ifdef __linux__

JzRE::JzSaveFileDialog::JzSaveFileDialog(const JzRE::String &p_dialogTitle) :
    JzFileDialog(p_dialogTitle) { }

void JzRE::JzSaveFileDialog::DefineExtension(const JzRE::String &p_label, const JzRE::String &p_extension)
{
    m_filter    = p_label + '\0' + '*' + p_extension + '\0';
    m_extension = p_extension;
}

void JzRE::JzSaveFileDialog::Show()
{
    JzFileDialog::Show();

    if (m_succeeded)
        _addExtensionToFilePathAndName();
}

void JzRE::JzSaveFileDialog::_addExtensionToFilePathAndName()
{
    if (m_filename.size() >= m_extension.size()) {
        String fileEnd(m_filename.data() + m_filename.size() - m_extension.size(), m_filename.data() + m_filename.size());

        if (fileEnd != m_extension) {
            m_filepath += m_extension;
            m_filename += m_extension;
        }
    } else {
        m_filepath += m_extension;
        m_filename += m_extension;
    }
}

#endif // __linux__