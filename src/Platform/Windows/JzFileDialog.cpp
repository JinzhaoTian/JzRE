/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzFileDialog.h"

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#include <shobjidl.h>
#include <filesystem>

JzRE::JzFileDialog::JzFileDialog(const JzRE::String &p_dialogTitle) :
    m_dialogTitle(p_dialogTitle),
    m_initialDirectory(""),
    m_succeeded(false) { }

void JzRE::JzFileDialog::SetInitialDirectory(const JzRE::String &p_initialDirectory)
{
    m_initialDirectory = p_initialDirectory;
}

void JzRE::JzFileDialog::Show(JzEFileDialogType type)
{
    m_succeeded = false;
    m_filepath.clear();
    m_filename.clear();

    IFileDialog *pfd;
    CLSID        clsid = (type == JzEFileDialogType::SaveFile) ? CLSID_FileSaveDialog : CLSID_FileOpenDialog;
    HRESULT      hr    = CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));

    if (SUCCEEDED(hr)) {
        if (type == JzEFileDialogType::OpenFolder) {
            DWORD dwOptions;
            pfd->GetOptions(&dwOptions);
            pfd->SetOptions(dwOptions | FOS_PICKFOLDERS);
        } else {
            if (!m_filter.empty()) {
                std::vector<COMDLG_FILTERSPEC> filters;
                COMDLG_FILTERSPEC              spec = {L"All Files", L"*.*"}; // TODO
                filters.push_back(spec);
                pfd->SetFileTypes(static_cast<UINT>(filters.size()), filters.data());
            }
        }

        if (!m_dialogTitle.empty()) {
            pfd->SetTitle(std::wstring(m_dialogTitle.begin(), m_dialogTitle.end()).c_str());
        }

        if (!m_initialDirectory.empty()) {
            IShellItem *psiFolder;
            hr = SHCreateItemFromParsingName(std::wstring(m_initialDirectory.begin(), m_initialDirectory.end()).c_str(),
                                             NULL, IID_PPV_ARGS(&psiFolder));
            if (SUCCEEDED(hr)) {
                pfd->SetFolder(psiFolder);
                psiFolder->Release();
            }
        }

        hr = pfd->Show(NULL);
        if (SUCCEEDED(hr)) {
            IShellItem *psiResult;
            hr = pfd->GetResult(&psiResult);
            if (SUCCEEDED(hr)) {
                PWSTR pszFilePath;
                hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                if (SUCCEEDED(hr)) {
                    // 转换为多字节字符串
                    int size = WideCharToMultiByte(CP_UTF8, 0, pszFilePath, -1, NULL, 0, NULL, NULL);
                    m_filepath.resize(size);
                    WideCharToMultiByte(CP_UTF8, 0, pszFilePath, -1, &m_filepath[0], size, NULL, NULL);
                    m_succeeded = true;
                    CoTaskMemFree(pszFilePath);
                }
                psiResult->Release();
            }
        }
        pfd->Release();
    }

    if (m_succeeded) {
        m_filename.clear();
        if (!m_filepath.empty()) {
            size_t lastSlash = m_filepath.find_last_of("\\/");
            if (lastSlash != std::string::npos) {
                m_filename = m_filepath.substr(lastSlash + 1);
            } else {
                m_filename = m_filepath;
            }
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

void JzRE::JzFileDialog::HandleError()
{
    // TODO: Implement proper error handling
    m_error = "file dialog error occurred";
}

#endif // _WIN32
