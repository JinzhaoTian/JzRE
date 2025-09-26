/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Platform/JzOpenFileDialog.h"

#ifdef _WIN32

JzRE::JzOpenFileDialog::JzOpenFileDialog(const JzRE::String &p_dialogTitle) :
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

#endif // _WIN32
