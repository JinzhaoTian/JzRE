/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Core/JzFileSystemUtils.h"
#include <algorithm>

JzRE::String JzRE::JzFileSystemUtils::MakeWindowsStyle(const JzRE::String &p_path)
{
    String result;
    result.resize(p_path.size());

    for (Size i = 0; i < p_path.size(); ++i)
        result[i] = p_path[i] == '/' ? '\\' : p_path[i];

    return result;
}

JzRE::String JzRE::JzFileSystemUtils::MakeNonWindowsStyle(const JzRE::String &p_path)
{
    String result;
    result.resize(p_path.size());

    for (size_t i = 0; i < p_path.size(); ++i)
        result[i] = p_path[i] == '\\' ? '/' : p_path[i];

    return result;
}

JzRE::String JzRE::JzFileSystemUtils::GetContainingFolder(const JzRE::String &p_path)
{
    String result;

    bool extraction = false;

    for (auto it = p_path.rbegin(); it != p_path.rend(); ++it) {
        if (extraction)
            result += *it;

        if (!extraction && it != p_path.rbegin() && (*it == '\\' || *it == '/'))
            extraction = true;
    }

    std::reverse(result.begin(), result.end());

    if (!result.empty() && result.back() != '\\')
        result += '\\';

    return result;
}

JzRE::String JzRE::JzFileSystemUtils::GetElementName(const JzRE::String &p_path)
{
    String result;

    String path = p_path;
    if (!path.empty() && path.back() == '\\')
        path.pop_back();

    for (auto it = path.rbegin(); it != path.rend() && *it != '\\' && *it != '/'; ++it)
        result += *it;

    std::reverse(result.begin(), result.end());

    return result;
}

JzRE::String JzRE::JzFileSystemUtils::GetExtension(const JzRE::String &p_path)
{
    String result;

    for (auto it = p_path.rbegin(); it != p_path.rend() && *it != '.'; ++it)
        result += *it;

    std::reverse(result.begin(), result.end());

    return result;
}

JzRE::String JzRE::JzFileSystemUtils::FileTypeToString(JzRE::JzEFileType p_fileType)
{
    switch (p_fileType) {
        case JzRE::JzEFileType::MODEL: return "Model";
        case JzRE::JzEFileType::TEXTURE: return "Texture";
        case JzRE::JzEFileType::SHADER: return "Shader";
        case JzRE::JzEFileType::SHADER_PART: return "Shader_Part";
        case JzRE::JzEFileType::MATERIAL: return "Material";
        case JzRE::JzEFileType::SOUND: return "Sound";
        case JzRE::JzEFileType::SCENE: return "Scene";
        case JzRE::JzEFileType::SCRIPT: return "Script";
        case JzRE::JzEFileType::FONT: return "Font";
        case JzRE::JzEFileType::UNKNOWN: return "Unknown";
    }

    return "Unknown";
}

JzRE::JzEFileType JzRE::JzFileSystemUtils::GetFileType(const JzRE::String &p_path)
{
    String loweredPath = p_path;
    std::transform(loweredPath.begin(), loweredPath.end(), loweredPath.begin(), ::tolower);
    if (loweredPath.ends_with(".jzshader.src.json"))
        return JzEFileType::SHADER_PART;

    String ext = GetExtension(p_path);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    if (ext == "fbx" || ext == "obj")
        return JzEFileType::MODEL;
    else if (ext == "png" || ext == "jpeg" || ext == "jpg" || ext == "tga" || ext == "hdr")
        return JzEFileType::TEXTURE;
    else if (ext == "jzshader")
        return JzEFileType::SHADER;
    else if (ext == "jzsblob" || ext == "hlsl" || ext == "hlsli")
        return JzEFileType::SHADER_PART;
    else if (ext == "ovmat")
        return JzEFileType::MATERIAL;
    else if (ext == "wav" || ext == "mp3" || ext == "ogg")
        return JzEFileType::SOUND;
    else if (ext == "ovscene")
        return JzEFileType::SCENE;
    else if (ext == "lua" || ext == "ovscript")
        return JzEFileType::SCRIPT;
    else if (ext == "ttf")
        return JzEFileType::FONT;

    return JzEFileType::UNKNOWN;
}
