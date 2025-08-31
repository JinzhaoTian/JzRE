#include "JzOpenFileDialog.h"

JzRE::JzOpenFileDialog::JzOpenFileDialog(const JzRE::String &p_dialogTitle) :
    m_fileDialog(JzRE::JzFileDialog::Create(p_dialogTitle)) { }

void JzRE::JzOpenFileDialog::SetInitialDirectory(const JzRE::String &p_initialDirectory)
{
    if (m_fileDialog) {
        m_fileDialog->SetInitialDirectory(p_initialDirectory);
    }
}

void JzRE::JzOpenFileDialog::SetOpenFileType(JzRE::Bool openFile)
{
    if (m_fileDialog) {
        m_fileDialog->SetOpenFileType(openFile);
    }
}

void JzRE::JzOpenFileDialog::Show()
{
    if (m_fileDialog) {
        m_fileDialog->Show();
    }
}

JzRE::Bool JzRE::JzOpenFileDialog::HasSucceeded() const
{
    return m_fileDialog ? m_fileDialog->HasSucceeded() : false;
}

JzRE::String JzRE::JzOpenFileDialog::GetSelectedFileName()
{
    return m_fileDialog ? m_fileDialog->GetSelectedFileName() : "";
}

JzRE::String JzRE::JzOpenFileDialog::GetSelectedFilePath()
{
    return m_fileDialog ? m_fileDialog->GetSelectedFilePath() : "";
}

JzRE::String JzRE::JzOpenFileDialog::GetErrorInfo()
{
    return m_fileDialog ? m_fileDialog->GetErrorInfo() : "File dialog not initialized";
}

JzRE::Bool JzRE::JzOpenFileDialog::IsFileExisting() const
{
    return m_fileDialog ? m_fileDialog->IsFileExisting() : false;
}

void JzRE::JzOpenFileDialog::AddFileType(const JzRE::String &p_label, const JzRE::String &p_filter)
{
    if (m_fileDialog) {
        m_fileDialog->AddFileType(p_label, p_filter);
    }
}
