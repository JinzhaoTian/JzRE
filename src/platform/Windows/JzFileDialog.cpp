/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzFileDialog.h"

#ifdef _WIN32
#include <windows.h>
#include <commdlg.h>
#include <filesystem>

JzRE::JzFileDialog::JzFileDialog(const JzRE::String &p_dialogTitle) :
    m_dialogTitle(p_dialogTitle),
    m_initialDirectory(""),
    m_succeeded(false) { }

void JzRE::JzFileDialog::SetInitialDirectory(const JzRE::String &p_initialDirectory)
{
    m_initialDirectory = p_initialDirectory;
}

void JzRE::JzFileDialog::Show()
{
    m_succeeded = false;
    m_filepath.clear();
    m_filename.clear();

    OPENFILENAMEA ofn;
    char          szFile[260] = {0};

    // Initialize OPENFILENAME
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner   = NULL;
    ofn.lpstrFile   = szFile;
    ofn.nMaxFile    = sizeof(szFile);

    // Prepare filter string (must end with double null)
    std::string filter = m_filter;
    if (!filter.empty() && filter.back() != '\0') {
        filter += '\0';
    }
    filter += '\0'; // Double null terminator

    ofn.lpstrFilter    = filter.c_str();
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle  = 0;

    if (!m_initialDirectory.empty()) {
        ofn.lpstrInitialDir = m_initialDirectory.c_str();
    }

    ofn.lpstrTitle = m_dialogTitle.c_str();
    ofn.Flags      = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    // Display the Open dialog box
    if (GetOpenFileNameA(&ofn) == TRUE) {
        m_filepath  = szFile;
        m_succeeded = true;
    } else {
        HandleError();
    }

    if (m_succeeded) {
        m_filename.clear();
        if (!m_filepath.empty()) {
            for (auto it = m_filepath.rbegin(); it != m_filepath.rend() && *it != '\\' && *it != '/'; ++it)
                m_filename += *it;
            std::reverse(m_filename.begin(), m_filename.end());
        }
    }
}

JzRE::Bool JzRE::JzFileDialog::HasSucceeded() const
{
    return m_succeeded;
}

JzRE::String JzRE::JzFileDialog::GetSelectedFileName()
{
    return m_filename;
}

JzRE::String JzRE::JzFileDialog::GetSelectedFilePath()
{
    return m_filepath;
}

JzRE::String JzRE::JzFileDialog::GetErrorInfo()
{
    return m_error;
}

JzRE::Bool JzRE::JzFileDialog::IsFileExisting() const
{
    return std::filesystem::exists(m_filepath);
}

void JzRE::JzOpenFileDialog::HandleError()
{
    // TODO: Implement proper error handling
    m_error = "file dialog error occurred";
}

#endif // _WIN32
