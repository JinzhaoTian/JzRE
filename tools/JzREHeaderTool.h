#pragma once

#include "JzREHeaderToolTypes.h"

#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <ostream>

/**
 * @brief JzRE Header Tool
 */
class JzREHeaderTool {
public:
    JzREHeaderTool() = default;

    void ProcessFiles(const std::vector<std::string> &sourceFiles);
    void AddReflectedClass(const JzREHeaderToolReflectedClassInfo &classInfo);

    void PreprocessPhase(const std::vector<std::string> &sourceFiles);
    void ParsePhase();
    void ValidatePhase();
    void GeneratePhase();
    void GeneratePhasePerFile(); // 新增：按文件生成反射代码
    void GenerateReflectionCode();
    void GenerateReflectionCodePerFile(); // 新增：为每个文件生成独立的反射代码

private:
    void                                         GenerateHeaderFile(const std::string &outputPath);
    void                                         GenerateSourceFile(const std::string &outputPath);
    void                                         GeneratePerFileReflectionFiles(const std::string &headerFile, const std::vector<JzREHeaderToolReflectedClassInfo> &classes); // 新增：为单个文件生成反射代码
    void                                         GenerateGeneratedHeaderFile(const std::string &headerFile, const std::vector<JzREHeaderToolReflectedClassInfo> &classes); // 新增：生成 .generated.h 文件
    void                                         GenerateClassReflection(std::ostream &headerStream, std::ostream &sourceStream, const JzREHeaderToolReflectedClassInfo &classInfo);
    void                                         GenerateGeneratedBodyMacro(std::ostream &headerStream, const JzREHeaderToolReflectedClassInfo &classInfo); // 新增：生成 GENERATED_BODY 宏内容
    std::string                                  SanitizeTypeName(const std::string &typeName);
    std::string                                  GetIncludeGuard(const std::string &fileName);
    std::string                                  GetRelativePath(const std::string &fullPath);
    std::string                                  GetFileBaseName(const std::string &filePath); // 新增：获取文件基名
    std::unordered_map<std::string, std::string> ParseAnnotationString(const std::string &annotation);
    bool                                         NeedsGeneratedInclude(const std::string &headerFile); // 新增：检查是否需要生成 .generated.h

private:
    std::vector<std::string>                      inputFiles;
    std::vector<JzREHeaderToolReflectedClassInfo> reflectedClasses;
    std::unordered_set<std::string>               processedFiles;
};
