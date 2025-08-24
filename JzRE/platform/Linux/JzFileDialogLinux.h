#pragma once

#include "../../JzFileDialog.h"

#ifdef __linux__

namespace JzRE {

/**
 * @brief File Dialog Implementation of Linux-specific
 */
class JzFileDialogLinux : public JzFileDialog {
public:
    /**
     * @brief Constructor
     *
     * @param p_dialogTitle Dialog window title
     */
    explicit JzFileDialogLinux(const String &p_dialogTitle);

    /**
     * @brief Destructor
     */
    ~JzFileDialogLinux() override = default;

    /**
     * @brief Set the initial directory
     *
     * @param p_initialDirectory The initial directory
     */
    void SetInitialDirectory(const String &p_initialDirectory) override;

    /**
     * @brief Set Open File or Folder
     *
     * @param openFile true if open file, false if open file folder
     */
    void SetOpenFileType(Bool openFile) override;

    /**
     * @brief Show the file dialog
     */
    void Show() override;

    /**
     * @brief Check if the file dialog has succeeded
     *
     * @return True if the file dialog has succeeded, false otherwise
     */
    Bool HasSucceeded() const override;

    /**
     * @brief Get the selected file name
     *
     * @return The selected file name
     */
    String GetSelectedFileName() override;

    /**
     * @brief Get the selected file path
     *
     * @return The selected file path
     */
    String GetSelectedFilePath() override;

    /**
     * @brief Get the error information
     *
     * @return The error information
     */
    String GetErrorInfo() override;

    /**
     * @brief Check if the file exists
     *
     * @return True if the file exists, false otherwise
     */
    Bool IsFileExisting() const override;

    /**
     * @brief Add a file type
     *
     * @param p_label The label
     * @param p_filter The filter
     */
    void AddFileType(const String &p_label, const String &p_filter) override;

private:
    void HandleError();
    void ShowFallback();

private:
    String m_dialogTitle;
    String m_initialDirectory;
    String m_filter;
    String m_error;
    String m_filename;
    String m_filepath;
    Bool   m_succeeded;
    Bool   m_openFile;
};

} // namespace JzRE

#endif // __linux__
