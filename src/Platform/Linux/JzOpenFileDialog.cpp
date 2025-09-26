/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Platform/JzOpenFileDialog.h"

#ifdef __linux__

JzRE::JzOpenFileDialog::JzFileDialogLinux(const JzRE::String &p_dialogTitle) :
    JzFileDialog(p_dialogTitle) { }

void JzRE::JzOpenFileDialog::AddFileType(const JzRE::String &p_label, const JzRE::String &p_filter)
{
    if (!m_filter.empty()) {
        m_filter += "|";
    }
    m_filter += p_label + ":" + p_filter;
}

void JzRE::JzOpenFileDialog::Show(JzRE::JzEFileDialogType type)
{
    JzFileDialog::Show(type);
}

#endif // __linux__
