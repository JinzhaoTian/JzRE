/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#ifdef _WIN32

#include "JzRE/Platform/JzOpenFileDialog.h"

JzRE::JzOpenFileDialog::JzOpenFileDialog(const JzRE::String &dialogTitle) :
    JzFileDialog(dialogTitle) { }

void JzRE::JzOpenFileDialog::AddFileType(const JzRE::String &label, const JzRE::String &filter)
{
    if (!m_filter.empty()) {
        m_filter += "|";
    }
    m_filter += label + ":" + filter;
}

void JzRE::JzOpenFileDialog::Show(JzRE::JzEFileDialogType type)
{
    JzFileDialog::Show(type);
}

#endif // _WIN32
