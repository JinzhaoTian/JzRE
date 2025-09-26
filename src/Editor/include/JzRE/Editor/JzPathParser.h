/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Core/JzRETypes.h"
#include "JzRE/Core/JzEFile.h"

namespace JzRE {
class JzPathParser {
public:
    /**
     * Disabled constructor
     */
    JzPathParser() = delete;

    /**
     * Returns the windows style version of the given path ('/' replaced by '\')
     * @param p_path
     */
    static String MakeWindowsStyle(const String &p_path);

    /**
     * Returns the non-windows style version of the given path ('\' replaced by '/')
     * @param p_path
     */
    static String MakeNonWindowsStyle(const String &p_path);

    /**
     * Returns the containing folder of the file or folder identified by the given path
     * @param p_path
     */
    static String GetContainingFolder(const String &p_path);

    /**
     * Returns the name of the file or folder identified by the given path
     * @param p_path
     */
    static String GetElementName(const String &p_path);

    /**
     * Returns the extension of the file or folder identified by the given path
     * @param p_path
     */
    static String GetExtension(const String &p_path);

    /**
     * Convert the EFileType value to a string
     * @param p_fileType
     */
    static String FileTypeToString(JzEFileType p_fileType);

    /**
     * Returns the file type of the file identified by the given path
     * @param p_path
     */
    static JzEFileType GetFileType(const String &p_path);
};

} // namespace JzRE