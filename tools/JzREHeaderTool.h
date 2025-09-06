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
    void GenerateReflectionCode();

private:
    void                                         GenerateHeaderFile(const std::string &outputPath);
    void                                         GenerateSourceFile(const std::string &outputPath);
    void                                         GenerateClassReflection(std::ostream &headerStream, std::ostream &sourceStream, const JzREHeaderToolReflectedClassInfo &classInfo);
    std::string                                  SanitizeTypeName(const std::string &typeName);
    std::string                                  GetIncludeGuard(const std::string &fileName);
    std::string                                  GetRelativePath(const std::string &fullPath);
    std::unordered_map<std::string, std::string> ParseAnnotationString(const std::string &annotation);

private:
    std::vector<std::string>                      inputFiles;
    std::vector<JzREHeaderToolReflectedClassInfo> reflectedClasses;
    std::unordered_set<std::string>               processedFiles;
};
