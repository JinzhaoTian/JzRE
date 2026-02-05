/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#ifdef _WIN32

#include "JzRE/Runtime/Platform/Dialog/JzFileDialog.h"

#define NOMINMAX
#include <windows.h>
#include <shobjidl.h>

JzRE::JzFileDialog::JzFileDialog(const JzRE::String &dialogTitle) :
    m_dialogTitle(dialogTitle),
    m_succeeded(false) { }

void JzRE::JzFileDialog::SetInitialDirectory(const std::filesystem::path &initialDirectory)
{
    m_initialDirectory = initialDirectory;
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
            auto filterEntries = ParseFilters();
            if (!filterEntries.empty()) {
                std::vector<COMDLG_FILTERSPEC> filters;
                std::vector<std::wstring>      labels;
                std::vector<std::wstring>      patterns;

                for (const auto &entry : filterEntries) {
                    labels.push_back(std::wstring(entry.label.begin(), entry.label.end()));
                    patterns.push_back(std::wstring(entry.pattern.begin(), entry.pattern.end()));
                }

                // Build COMDLG_FILTERSPEC array (must reference stable wstring storage)
                for (size_t i = 0; i < labels.size(); ++i) {
                    COMDLG_FILTERSPEC spec = {labels[i].c_str(), patterns[i].c_str()};
                    filters.push_back(spec);
                }

                if (!filters.empty()) {
                    pfd->SetFileTypes(static_cast<UINT>(filters.size()), filters.data());
                }
            }
        }

        if (!m_dialogTitle.empty()) {
            pfd->SetTitle(std::wstring(m_dialogTitle.begin(), m_dialogTitle.end()).c_str());
        }

        if (!m_initialDirectory.empty()) {
            IShellItem *psiFolder;
            hr = SHCreateItemFromParsingName(m_initialDirectory.c_str(), NULL, IID_PPV_ARGS(&psiFolder));
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
                    m_filepath  = pszFilePath;
                    m_succeeded = true;
                    CoTaskMemFree(pszFilePath);
                }
                psiResult->Release();
            }
        }
        pfd->Release();
    }

    if (m_succeeded) {
        if (!m_filepath.empty()) {
            m_filename = m_filepath.filename();
        }
    }
}

JzRE::Bool JzRE::JzFileDialog::HasSucceeded() const
{
    return m_succeeded;
}

std::filesystem::path JzRE::JzFileDialog::GetSelectedFileName()
{
    return m_filename;
}

std::filesystem::path JzRE::JzFileDialog::GetSelectedFilePath()
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
