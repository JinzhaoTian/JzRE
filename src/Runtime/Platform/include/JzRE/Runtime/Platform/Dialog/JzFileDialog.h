/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <filesystem>
#include <vector>
#include "JzRE/Runtime/Core/JzRETypes.h"

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
 * @brief Parsed file filter entry
 */
struct JzFileFilterEntry {
    String              label;      ///< Display label (e.g., "JzRE Project")
    String              pattern;    ///< Full pattern string (e.g., "*.jzreproject")
    std::vector<String> extensions; ///< Parsed extensions without "*." (e.g., {"jzreproject"})
};

/**
 * @brief File Dialog Interface
 */
class JzFileDialog {
public:
    /**
     * @brief Constructor
     *
     * @param dialogTitle
     */
    JzFileDialog(const String &dialogTitle);

    /**
     * @brief Destructor
     */
    virtual ~JzFileDialog() = default;

    /**
     * @brief Defines the initial directory (Where the FileDialog will open)
     *
     * @param initialDirectory
     */
    void SetInitialDirectory(const std::filesystem::path &initialDirectory);

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
    std::filesystem::path GetSelectedFileName();

    /**
     * @brief Get the selected file path (Make sure that HasSucceeded() returned true before calling this method)
     *
     * @return selected file path
     */
    std::filesystem::path GetSelectedFilePath();

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
    /**
     * @brief Parse the filter string into structured entries
     *
     * Filter format: "Label1:*.ext1;*.ext2|Label2:*.ext3"
     * - Multiple filters separated by "|"
     * - Each filter: "Label:pattern"
     * - Pattern can contain multiple extensions separated by ";"
     *
     * @return Vector of parsed filter entries
     */
    std::vector<JzFileFilterEntry> ParseFilters() const
    {
        std::vector<JzFileFilterEntry> entries;
        if (m_filter.empty()) {
            return entries;
        }

        // to parse a single filter item
        auto parseItem = [](const String &item) -> JzFileFilterEntry {
            JzFileFilterEntry entry;
            size_t            colonPos = item.find(':');
            if (colonPos != String::npos) {
                entry.label   = item.substr(0, colonPos);
                entry.pattern = item.substr(colonPos + 1);

                // Parse extensions from pattern (e.g., "*.txt;*.doc" -> {"txt", "doc"})
                String extensions = entry.pattern;
                size_t extPos     = 0;
                while ((extPos = extensions.find("*.")) != String::npos) {
                    extensions.erase(extPos, 2); // Remove "*."
                    size_t semicolonPos = extensions.find(';');
                    String ext          = extensions.substr(0, semicolonPos);
                    if (!ext.empty()) {
                        entry.extensions.push_back(ext);
                    }
                    if (semicolonPos != String::npos) {
                        extensions = extensions.substr(semicolonPos + 1);
                    } else {
                        break;
                    }
                }
            }
            return entry;
        };

        // Parse multiple filter items separated by "|"
        String filter = m_filter;
        size_t pos    = 0;
        while ((pos = filter.find('|')) != String::npos) {
            String item = filter.substr(0, pos);
            if (!item.empty()) {
                entries.push_back(parseItem(item));
            }
            filter.erase(0, pos + 1);
        }

        // Parse the last (or only) filter item
        if (!filter.empty()) {
            entries.push_back(parseItem(filter));
        }

        return entries;
    }

protected:
    std::filesystem::path m_initialDirectory;
    String                m_dialogTitle;
    String                m_filter;
    std::filesystem::path m_filename;
    std::filesystem::path m_filepath;
    Bool                  m_succeeded;
    String                m_error;
};

} // namespace JzRE