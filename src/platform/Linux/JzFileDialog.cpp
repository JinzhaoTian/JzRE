/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzFileDialog.h"

#ifdef __linux__
#include <gtk/gtk.h>
#include <filesystem>
#include <iostream>

JzRE::JzFileDialog::JzFileDialog(const JzRE::String &p_dialogTitle) :
    m_dialogTitle(p_dialogTitle),
    m_initialDirectory(""),
    m_succeeded(false) { }

void JzRE::JzOpenFileDialog::SetInitialDirectory(const JzRE::String &p_initialDirectory)
{
    m_initialDirectory = p_initialDirectory;
}

void JzRE::JzOpenFileDialog::Show()
{
    m_succeeded = false;
    m_filepath.clear();
    m_filename.clear();

    // Initialize GTK if not already initialized
    if (!gtk_init_check(NULL, NULL)) {
        ShowFallback();
        return;
    }

    GtkWidget *dialog = gtk_file_chooser_dialog_new(
        m_dialogTitle.c_str(),
        NULL,
        GTK_FILE_CHOOSER_ACTION_OPEN,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_Open", GTK_RESPONSE_ACCEPT,
        NULL);

    // Set initial directory
    if (!m_initialDirectory.empty()) {
        gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), m_initialDirectory.c_str());
    }

    // Parse and set file filters
    if (!m_filter.empty()) {
        std::string filter = m_filter;
        size_t      pos    = 0;

        while ((pos = filter.find("|")) != std::string::npos) {
            std::string item     = filter.substr(0, pos);
            size_t      colonPos = item.find(":");
            if (colonPos != std::string::npos) {
                std::string label   = item.substr(0, colonPos);
                std::string pattern = item.substr(colonPos + 1);

                GtkFileFilter *fileFilter = gtk_file_filter_new();
                gtk_file_filter_set_name(fileFilter, label.c_str());
                gtk_file_filter_add_pattern(fileFilter, pattern.c_str());
                gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), fileFilter);
            }
            filter.erase(0, pos + 1);
        }
    }

    gint result = gtk_dialog_run(GTK_DIALOG(dialog));
    if (result == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        m_filepath     = filename;
        g_free(filename);
        m_succeeded = true;
    }

    gtk_widget_destroy(dialog);

    // Run GTK main iteration to process events
    while (gtk_events_pending()) {
        gtk_main_iteration();
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

void JzRE::JzFileDialog::HandleError()
{
    // TODO: Implement proper error handling
    m_error = "file dialog error occurred";
}

#endif // __linux__
