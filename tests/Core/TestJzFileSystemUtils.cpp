/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#include <gtest/gtest.h>

#include "JzRE/Runtime/Core/JzFileSystemUtils.h"

using namespace JzRE;

// ---------------------------------------------------------------------------
// MakeWindowsStyle
// ---------------------------------------------------------------------------

TEST(JzFileSystemUtils, MakeWindowsStyleConvertsForwardSlashes)
{
    EXPECT_EQ(JzFileSystemUtils::MakeWindowsStyle("path/to/file.txt"),
              "path\\to\\file.txt");
}

TEST(JzFileSystemUtils, MakeWindowsStyleLeavesBackslashesUnchanged)
{
    EXPECT_EQ(JzFileSystemUtils::MakeWindowsStyle("path\\to\\file.txt"),
              "path\\to\\file.txt");
}

TEST(JzFileSystemUtils, MakeWindowsStyleEmptyStringReturnsEmpty)
{
    EXPECT_EQ(JzFileSystemUtils::MakeWindowsStyle(""), "");
}

TEST(JzFileSystemUtils, MakeWindowsStyleNoSeparatorUnchanged)
{
    EXPECT_EQ(JzFileSystemUtils::MakeWindowsStyle("filename.txt"), "filename.txt");
}

// ---------------------------------------------------------------------------
// MakeNonWindowsStyle
// ---------------------------------------------------------------------------

TEST(JzFileSystemUtils, MakeNonWindowsStyleConvertsBackslashes)
{
    EXPECT_EQ(JzFileSystemUtils::MakeNonWindowsStyle("path\\to\\file.txt"),
              "path/to/file.txt");
}

TEST(JzFileSystemUtils, MakeNonWindowsStyleLeavesForwardSlashesUnchanged)
{
    EXPECT_EQ(JzFileSystemUtils::MakeNonWindowsStyle("path/to/file.txt"),
              "path/to/file.txt");
}

TEST(JzFileSystemUtils, MakeNonWindowsStyleEmptyStringReturnsEmpty)
{
    EXPECT_EQ(JzFileSystemUtils::MakeNonWindowsStyle(""), "");
}

// ---------------------------------------------------------------------------
// GetContainingFolder
// ---------------------------------------------------------------------------

TEST(JzFileSystemUtils, GetContainingFolderReturnsDirectoryWithTrailingSeparator)
{
    // "path/to/file.txt" -> "path/to\"  (implementation always appends '\')
    const String result = JzFileSystemUtils::GetContainingFolder("path/to/file.txt");
    EXPECT_EQ(result, "path/to\\");
}

TEST(JzFileSystemUtils, GetContainingFolderWindowsPath)
{
    const String result = JzFileSystemUtils::GetContainingFolder("C:\\Users\\file.txt");
    EXPECT_EQ(result, "C:\\Users\\");
}

TEST(JzFileSystemUtils, GetContainingFolderNoDirectoryReturnsEmpty)
{
    // just a filename, no separator -> empty
    const String result = JzFileSystemUtils::GetContainingFolder("file.txt");
    EXPECT_EQ(result, "");
}

// ---------------------------------------------------------------------------
// GetElementName
// ---------------------------------------------------------------------------

TEST(JzFileSystemUtils, GetElementNameExtractsFilename)
{
    EXPECT_EQ(JzFileSystemUtils::GetElementName("path/to/file.txt"), "file.txt");
}

TEST(JzFileSystemUtils, GetElementNameWindowsPath)
{
    EXPECT_EQ(JzFileSystemUtils::GetElementName("C:\\Users\\doc.pdf"), "doc.pdf");
}

TEST(JzFileSystemUtils, GetElementNameNoDirectoryReturnsSelf)
{
    EXPECT_EQ(JzFileSystemUtils::GetElementName("standalone.obj"), "standalone.obj");
}

TEST(JzFileSystemUtils, GetElementNameTrailingBackslashStripped)
{
    // The implementation strips a trailing '\\' before extraction
    EXPECT_EQ(JzFileSystemUtils::GetElementName("path\\folder\\"), "folder");
}

TEST(JzFileSystemUtils, GetElementNameEmptyStringReturnsEmpty)
{
    EXPECT_EQ(JzFileSystemUtils::GetElementName(""), "");
}

// ---------------------------------------------------------------------------
// GetExtension
// ---------------------------------------------------------------------------

TEST(JzFileSystemUtils, GetExtensionReturnsPartAfterDot)
{
    EXPECT_EQ(JzFileSystemUtils::GetExtension("file.txt"), "txt");
    EXPECT_EQ(JzFileSystemUtils::GetExtension("model.fbx"), "fbx");
    EXPECT_EQ(JzFileSystemUtils::GetExtension("shader.jzshader"), "jzshader");
}

TEST(JzFileSystemUtils, GetExtensionWithFullPath)
{
    EXPECT_EQ(JzFileSystemUtils::GetExtension("path/to/texture.png"), "png");
}

TEST(JzFileSystemUtils, GetExtensionNoDotReturnsWholeString)
{
    // No '.' -> iterator never stops, entire string collected in reverse
    EXPECT_EQ(JzFileSystemUtils::GetExtension("nodotfile"), "nodotfile");
}

// ---------------------------------------------------------------------------
// GetFileType
// ---------------------------------------------------------------------------

TEST(JzFileSystemUtils, GetFileTypeModel)
{
    EXPECT_EQ(JzFileSystemUtils::GetFileType("mesh.fbx"), JzEFileType::MODEL);
    EXPECT_EQ(JzFileSystemUtils::GetFileType("scene.obj"), JzEFileType::MODEL);
}

TEST(JzFileSystemUtils, GetFileTypeTexture)
{
    EXPECT_EQ(JzFileSystemUtils::GetFileType("tex.png"), JzEFileType::TEXTURE);
    EXPECT_EQ(JzFileSystemUtils::GetFileType("image.jpg"), JzEFileType::TEXTURE);
    EXPECT_EQ(JzFileSystemUtils::GetFileType("image.jpeg"), JzEFileType::TEXTURE);
    EXPECT_EQ(JzFileSystemUtils::GetFileType("sprite.tga"), JzEFileType::TEXTURE);
    EXPECT_EQ(JzFileSystemUtils::GetFileType("envmap.hdr"), JzEFileType::TEXTURE);
}

TEST(JzFileSystemUtils, GetFileTypeShader)
{
    EXPECT_EQ(JzFileSystemUtils::GetFileType("material.jzshader"), JzEFileType::SHADER);
}

TEST(JzFileSystemUtils, GetFileTypeShaderPart)
{
    EXPECT_EQ(JzFileSystemUtils::GetFileType("blob.jzsblob"), JzEFileType::SHADER_PART);
    EXPECT_EQ(JzFileSystemUtils::GetFileType("vertex.hlsl"), JzEFileType::SHADER_PART);
    EXPECT_EQ(JzFileSystemUtils::GetFileType("include.hlsli"), JzEFileType::SHADER_PART);
}

TEST(JzFileSystemUtils, GetFileTypeShaderPartByFullSuffix)
{
    // Special case: path ending with ".jzshader.src.json"
    EXPECT_EQ(JzFileSystemUtils::GetFileType("unit.jzshader.src.json"), JzEFileType::SHADER_PART);
}

TEST(JzFileSystemUtils, GetFileTypeSound)
{
    EXPECT_EQ(JzFileSystemUtils::GetFileType("music.wav"), JzEFileType::SOUND);
    EXPECT_EQ(JzFileSystemUtils::GetFileType("track.mp3"), JzEFileType::SOUND);
    EXPECT_EQ(JzFileSystemUtils::GetFileType("sfx.ogg"), JzEFileType::SOUND);
}

TEST(JzFileSystemUtils, GetFileTypeFont)
{
    EXPECT_EQ(JzFileSystemUtils::GetFileType("font.ttf"), JzEFileType::FONT);
}

TEST(JzFileSystemUtils, GetFileTypeUnknownExtension)
{
    EXPECT_EQ(JzFileSystemUtils::GetFileType("data.xyz"), JzEFileType::UNKNOWN);
    EXPECT_EQ(JzFileSystemUtils::GetFileType("readme.md"), JzEFileType::UNKNOWN);
}

TEST(JzFileSystemUtils, GetFileTypeCaseInsensitiveExtension)
{
    EXPECT_EQ(JzFileSystemUtils::GetFileType("Texture.PNG"), JzEFileType::TEXTURE);
    EXPECT_EQ(JzFileSystemUtils::GetFileType("Model.FBX"), JzEFileType::MODEL);
}

// ---------------------------------------------------------------------------
// FileTypeToString
// ---------------------------------------------------------------------------

TEST(JzFileSystemUtils, FileTypeToStringConvertsAllTypes)
{
    EXPECT_EQ(JzFileSystemUtils::FileTypeToString(JzEFileType::MODEL),       "Model");
    EXPECT_EQ(JzFileSystemUtils::FileTypeToString(JzEFileType::TEXTURE),     "Texture");
    EXPECT_EQ(JzFileSystemUtils::FileTypeToString(JzEFileType::SHADER),      "Shader");
    EXPECT_EQ(JzFileSystemUtils::FileTypeToString(JzEFileType::SHADER_PART), "Shader_Part");
    EXPECT_EQ(JzFileSystemUtils::FileTypeToString(JzEFileType::MATERIAL),    "Material");
    EXPECT_EQ(JzFileSystemUtils::FileTypeToString(JzEFileType::SOUND),       "Sound");
    EXPECT_EQ(JzFileSystemUtils::FileTypeToString(JzEFileType::SCENE),       "Scene");
    EXPECT_EQ(JzFileSystemUtils::FileTypeToString(JzEFileType::SCRIPT),      "Script");
    EXPECT_EQ(JzFileSystemUtils::FileTypeToString(JzEFileType::FONT),        "Font");
    EXPECT_EQ(JzFileSystemUtils::FileTypeToString(JzEFileType::UNKNOWN),     "Unknown");
}
