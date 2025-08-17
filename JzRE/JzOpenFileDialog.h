#pragma once

#include "CommonTypes.h"
#include "JzFileDialog.h"

namespace JzRE {
/**
 * @brief Open File Dialog
 */
class JzOpenFileDialog {
public:
    /**
     * @brief Constructor
     *
     * @param p_dialogTitle Title of the dialog window
     */
    JzOpenFileDialog(const String &p_dialogTitle);

    /**
     * @brief Destructor
     */
    ~JzOpenFileDialog() = default;

    /**
     * @brief Defines the initial directory (Where the FileDialog will open)
     *
     * @param p_initialDirectory
     */
    void SetInitialDirectory(const String &p_initialDirectory);

    /**
     * @brief Show the file dialog
     */
    void Show();

    /**
     * @brief Has Succeeded
     *
     * @return true if the file action succeeded
     */
    Bool HasSucceeded() const;

    /**
     * @brief Get the selected file name (Make sure that HasSucceeded() returned true before calling this method)
     *
     * @return selected file name
     */
    String GetSelectedFileName();

    /**
     * @brief Get the selected file path (Make sure that HasSucceeded() returned true before calling this method)
     *
     * @return selected file path
     */
    String GetSelectedFilePath();

    /**
     * @brief Get some information about the last error (Make sure that HasSucceeded() returned false before calling this method)
     *
     * @return some information about the last error
     */
    String GetErrorInfo();

    /**
     * @brief Is the selected file exists
     *
     * @return true if the selected file exists
     */
    Bool IsFileExisting() const;

    /**
     * @brief Add a supported file type to the dialog window
     *
     * @param p_label
     * @param p_filter
     */
    void AddFileType(const String &p_label, const String &p_filter);

private:
    std::unique_ptr<JzFileDialog> m_fileDialog;
};
} // namespace JzRE