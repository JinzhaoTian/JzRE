/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Core/JzRETypes.h"

namespace JzRE {

/**
 * @brief Enums of File Dialog Types
 */
enum class JzEFileDialogType : U8 {
    DEFAULT,
    OpenFile,
    OpenFolder,
    SaveFile
};

/**
 * @brief File Dialog Interface
 */
class JzFileDialog {
public:
    /**
     * @brief Constructor
     *
     * @param p_dialogTitle
     */
    JzFileDialog(const String &p_dialogTitle);

    /**
     * @brief Destructor
     */
    virtual ~JzFileDialog() = default;

    /**
     * @brief Defines the initial directory (Where the FileDialog will open)
     *
     * @param p_initialDirectory
     */
    void SetInitialDirectory(const String &p_initialDirectory);

    /**
     * @brief Show the file dialog
     */
    virtual void Show(JzEFileDialogType type = JzEFileDialogType::DEFAULT);

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

private:
    void HandleError();

protected:
    String m_dialogTitle;
    String m_initialDirectory;
    String m_filter;
    String m_error;
    String m_filename;
    String m_filepath;
    Bool   m_succeeded;
};

} // namespace JzRE