/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <memory>
#include "JzRETypes.h"

namespace JzRE {

/**
 * @brief File Dialog Interface
 */
class JzFileDialog {
public:
    /**
     * @brief Factory method to create platform-specific file dialog
     *
     * @param p_dialogTitle Dialog window title
     * @return std::unique_ptr<JzFileDialog> Platform-specific implementation
     */
    static std::unique_ptr<JzFileDialog> Create(const String &p_dialogTitle);

    /**
     * @brief Destructor
     */
    virtual ~JzFileDialog() = default;

    /**
     * @brief Defines the initial directory (Where the FileDialog will open)
     *
     * @param p_initialDirectory
     */
    virtual void SetInitialDirectory(const String &p_initialDirectory) = 0;

    /**
     * @brief Set Open File or Folder
     *
     * @param openFile true if open file, false if open file folder
     */
    virtual void SetOpenFileType(Bool openFile) = 0;

    /**
     * @brief Show the file dialog
     */
    virtual void Show() = 0;

    /**
     * @brief Has Succeeded
     *
     * @return true if the file action succeeded
     */
    virtual Bool HasSucceeded() const = 0;

    /**
     * @brief Get the selected file name (Make sure that HasSucceeded() returned true before calling this method)
     *
     * @return selected file name
     */
    virtual String GetSelectedFileName() = 0;

    /**
     * @brief Get the selected file path (Make sure that HasSucceeded() returned true before calling this method)
     *
     * @return selected file path
     */
    virtual String GetSelectedFilePath() = 0;

    /**
     * @brief Get some information about the last error (Make sure that HasSucceeded() returned false before calling this method)
     *
     * @return some information about the last error
     */
    virtual String GetErrorInfo() = 0;

    /**
     * @brief Is the selected file exists
     *
     * @return true if the selected file exists
     */
    virtual Bool IsFileExisting() const = 0;

    /**
     * @brief Add a supported file type to the dialog window
     *
     * @param p_label
     * @param p_filter
     */
    virtual void AddFileType(const String &p_label, const String &p_filter) = 0;
};

} // namespace JzRE