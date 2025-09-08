#include "JzREHeaderTool.h"
#include "JzREHeaderToolOptions.h"

#include <fstream>
#include <regex>
#include <algorithm>
#include <cctype>

#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/ADT/StringRef.h"

void JzREHeaderTool::ProcessFiles(const std::vector<std::string> &sourceFiles)
{
    PreprocessPhase(sourceFiles);
    ParsePhase();
    ValidatePhase();
    GeneratePhase();
}

void JzREHeaderTool::AddReflectedClass(const JzREHeaderToolReflectedClassInfo &classInfo)
{
    reflectedClasses.push_back(classInfo);
}

void JzREHeaderTool::PreprocessPhase(const std::vector<std::string> &sourceFiles)
{
    if (JzREHeaderToolArgVerbose) {
        llvm::outs() << "=== Preprocessing Phase ===\n";
    }

    inputFiles = sourceFiles;

    // 收集所有需要处理的头文件
    for (const auto &file : sourceFiles) {
        if (llvm::sys::fs::exists(file)) {
            processedFiles.insert(file);
            if (JzREHeaderToolArgVerbose) {
                llvm::outs() << "Added file: " << file << "\n";
            }
        } else {
            llvm::errs() << "Warning: File not found: " << file << "\n";
        }
    }

    if (JzREHeaderToolArgVerbose) {
        llvm::outs() << "Total files to process: " << processedFiles.size() << "\n";
    }
}

void JzREHeaderTool::ParsePhase()
{
    if (JzREHeaderToolArgVerbose) {
        llvm::outs() << "=== Parsing Phase ===\n";
    }

    // 这个函数会被外部调用，通过Clang工具执行解析
}

void JzREHeaderTool::ValidatePhase()
{
    if (JzREHeaderToolArgVerbose) {
        llvm::outs() << "=== Validation Phase ===\n";
    }

    // 验证反射声明的一致性
    std::unordered_set<std::string> classNames;

    for (const auto &classInfo : reflectedClasses) {
        // 检查类名冲突
        if (classNames.find(classInfo.GetQualifiedName()) != classNames.end()) {
            llvm::errs() << "Error: Duplicate class name: " << classInfo.GetQualifiedName() << "\n";
        }
        classNames.insert(classInfo.GetQualifiedName());

        // 验证属性
        std::unordered_set<std::string> propertyNames;
        for (const auto &prop : classInfo.properties) {
            if (propertyNames.find(prop.name) != propertyNames.end()) {
                llvm::errs() << "Warning: Duplicate property name in " << classInfo.name
                             << ": " << prop.name << "\n";
            }
            propertyNames.insert(prop.name);
        }

        // 验证方法
        std::unordered_map<std::string, int> methodNames;
        for (const auto &method : classInfo.methods) {
            methodNames[method.name]++;
        }

        for (const auto &pair : methodNames) {
            if (pair.second > 1) {
                if (JzREHeaderToolArgVerbose) {
                    llvm::outs() << "Info: Overloaded method in " << classInfo.name
                                 << ": " << pair.first << " (" << pair.second << " overloads)\n";
                }
            }
        }
    }

    if (JzREHeaderToolArgVerbose) {
        llvm::outs() << "Validation completed. Found " << reflectedClasses.size()
                     << " reflected classes.\n";
    }
}

void JzREHeaderTool::GeneratePhase()
{
    if (JzREHeaderToolArgVerbose) {
        llvm::outs() << "=== Generation Phase ===\n";
    }

    GenerateReflectionCode();
}

void JzREHeaderTool::GeneratePhasePerFile()
{
    if (JzREHeaderToolArgVerbose) {
        llvm::outs() << "=== Generation Phase Per File ===\n";
    }

    GenerateReflectionCodePerFile();
}

void JzREHeaderTool::GenerateReflectionCode()
{
    if (JzREHeaderToolArgOutput.empty()) {
        llvm::errs() << "Error: Output directory not specified\n";
        return;
    }

    // 确保输出目录存在
    auto code = llvm::sys::fs::create_directories(JzREHeaderToolArgOutput);

    // TODO 每个文件生成一个
    std::string headerPath = JzREHeaderToolArgOutput + "/JzRE.generated.h";
    std::string sourcePath = JzREHeaderToolArgOutput + "/JzRE.generated.cpp";

    GenerateHeaderFile(headerPath);
    GenerateSourceFile(sourcePath);

    if (JzREHeaderToolArgVerbose) {
        llvm::outs() << "Generated files:\n";
        llvm::outs() << "  " << headerPath << "\n";
        llvm::outs() << "  " << sourcePath << "\n";
    }
}

void JzREHeaderTool::GenerateReflectionCodePerFile()
{
    if (JzREHeaderToolArgOutput.empty()) {
        llvm::errs() << "Error: Output directory not specified\n";
        return;
    }

    // 确保输出目录存在
    auto code = llvm::sys::fs::create_directories(JzREHeaderToolArgOutput);

    // 按文件分组反射类
    std::unordered_map<std::string, std::vector<JzREHeaderToolReflectedClassInfo>> fileToClasses;
    
    for (const auto &classInfo : reflectedClasses) {
        if (!classInfo.headerFile.empty()) {
            fileToClasses[classInfo.headerFile].push_back(classInfo);
        }
    }

    // 为每个文件生成反射代码
    for (const auto &pair : fileToClasses) {
        GenerateGeneratedHeaderFile(pair.first, pair.second);
        GeneratePerFileReflectionFiles(pair.first, pair.second);
    }

    if (JzREHeaderToolArgVerbose) {
        llvm::outs() << "Generated reflection files for " << fileToClasses.size() << " header files\n";
    }
}

void JzREHeaderTool::GenerateHeaderFile(const std::string &outputPath)
{
    std::ofstream headerFile(outputPath);
    if (!headerFile.is_open()) {
        llvm::errs() << "Error: Cannot create header file: " << outputPath << "\n";
        return;
    }

    // 生成头文件头部
    headerFile << "// This file is automatically generated by JzREHeaderTool\n";
    headerFile << "// DO NOT EDIT MANUALLY\n\n";
    headerFile << "#pragma once\n\n";
    headerFile << "#include \"JzReflectable.h\"\n";

    // 包含所有需要的头文件
    std::unordered_set<std::string> includedFiles;
    for (const auto &classInfo : reflectedClasses) {
        if (!classInfo.headerFile.empty()) {
            std::string relativePath = GetRelativePath(classInfo.headerFile);
            if (includedFiles.find(relativePath) == includedFiles.end()) {
                headerFile << "#include \"" << relativePath << "\"\n";
                includedFiles.insert(relativePath);
            }
        }
    }

    headerFile << "\nnamespace JzRE {\n\n";

    // 为每个类生成函数声明
    for (const auto &classInfo : reflectedClasses) {
        headerFile << "// Class: " << classInfo.GetQualifiedName() << "\n";
        headerFile << "const JzReflectedClassInfo& Get" << classInfo.name << "ClassInfo();\n\n";
    }

    headerFile << "} // namespace JzRE\n";
    headerFile.close();
}

void JzREHeaderTool::GenerateSourceFile(const std::string &outputPath)
{
    std::ofstream sourceFile(outputPath);
    if (!sourceFile.is_open()) {
        llvm::errs() << "Error: Cannot create source file: " << outputPath << "\n";
        return;
    }

    // 生成源文件头部
    sourceFile << "// This file is automatically generated by JzREHeaderTool\n";
    sourceFile << "// DO NOT EDIT MANUALLY\n\n";
    sourceFile << "#include \"JzRE.generated.h\"\n";
    sourceFile << "#include <cstddef>\n\n";
    sourceFile << "namespace JzRE {\n\n";

    // 为每个类生成反射信息
    for (const auto &classInfo : reflectedClasses) {
        GenerateClassReflection(sourceFile, sourceFile, classInfo);
    }

    sourceFile << "} // namespace JzRE\n";
    sourceFile.close();
}

void JzREHeaderTool::GeneratePerFileReflectionFiles(const std::string &headerFile, const std::vector<JzREHeaderToolReflectedClassInfo> &classes)
{
    if (classes.empty()) {
        return;
    }

    std::string fileBaseName = GetFileBaseName(headerFile);
    std::string headerPath = JzREHeaderToolArgOutput + "/" + fileBaseName + ".generated.h";
    std::string sourcePath = JzREHeaderToolArgOutput + "/" + fileBaseName + ".generated.cpp";

    // 生成头文件
    std::ofstream headerFileStream(headerPath);
    if (!headerFileStream.is_open()) {
        llvm::errs() << "Error: Cannot create header file: " << headerPath << "\n";
        return;
    }

    // 生成源文件
    std::ofstream sourceFileStream(sourcePath);
    if (!sourceFileStream.is_open()) {
        llvm::errs() << "Error: Cannot create source file: " << sourcePath << "\n";
        headerFileStream.close();
        return;
    }

    // 生成头文件内容
    headerFileStream << "// This file is automatically generated by JzREHeaderTool\n";
    headerFileStream << "// DO NOT EDIT MANUALLY\n\n";
    headerFileStream << "#pragma once\n\n";
    headerFileStream << "#include \"JzReflectable.h\"\n";
    headerFileStream << "#include \"" << GetRelativePath(headerFile) << "\"\n\n";
    headerFileStream << "namespace JzRE {\n\n";

    // 为每个类生成函数声明
    for (const auto &classInfo : classes) {
        headerFileStream << "// Class: " << classInfo.GetQualifiedName() << "\n";
        headerFileStream << "const JzReflectedClassInfo& Get" << classInfo.name << "ClassInfo();\n";
        headerFileStream << "void Register" << classInfo.name << "Reflection();\n\n";
    }

    headerFileStream << "} // namespace JzRE\n";
    headerFileStream.close();

    // 生成源文件内容
    sourceFileStream << "// This file is automatically generated by JzREHeaderTool\n";
    sourceFileStream << "// DO NOT EDIT MANUALLY\n\n";
    sourceFileStream << "#include \"" << fileBaseName << ".generated.h\"\n";
    sourceFileStream << "#include \"JzReflectionRegistry.h\"\n";
    sourceFileStream << "#include <cstddef>\n\n";

    // 为每个类生成静态成员函数实现
    for (const auto &classInfo : classes) {
        // 生成 GetStaticClass 实现
        sourceFileStream << "// Implementation for class: " << classInfo.GetQualifiedName() << "\n";
        sourceFileStream << "const ::JzRE::JzReflectedClassInfo& " << classInfo.GetQualifiedName() << "::GetStaticClass() {\n";
        sourceFileStream << "    return ::JzRE::Get" << classInfo.name << "ClassInfo();\n";
        sourceFileStream << "}\n\n";
        
        // 生成 RegisterReflection 实现
        sourceFileStream << "void " << classInfo.GetQualifiedName() << "::RegisterReflection() {\n";
        sourceFileStream << "    ::JzRE::Register" << classInfo.name << "Reflection();\n";
        sourceFileStream << "}\n\n";
    }

    sourceFileStream << "namespace JzRE {\n\n";

    // 为每个类生成反射信息
    for (const auto &classInfo : classes) {
        GenerateClassReflection(sourceFileStream, sourceFileStream, classInfo);
        
        // 生成注册函数
        sourceFileStream << "void Register" << classInfo.name << "Reflection() {\n";
        sourceFileStream << "    static bool registered = false;\n";
        sourceFileStream << "    if (!registered) {\n";
        sourceFileStream << "        JzReflectionRegistry::GetInstance().RegisterClass(Get" << classInfo.name << "ClassInfo());\n";
        sourceFileStream << "        registered = true;\n";
        sourceFileStream << "    }\n";
        sourceFileStream << "}\n\n";
    }

    // 生成自动注册代码
    sourceFileStream << "namespace {\n";
    sourceFileStream << "    static bool " << fileBaseName << "_auto_register = []() {\n";
    for (const auto &classInfo : classes) {
        sourceFileStream << "        Register" << classInfo.name << "Reflection();\n";
    }
    sourceFileStream << "        return true;\n";
    sourceFileStream << "    }();\n";
    sourceFileStream << "}\n\n";

    sourceFileStream << "} // namespace JzRE\n";
    sourceFileStream.close();

    if (JzREHeaderToolArgVerbose) {
        llvm::outs() << "Generated files for " << headerFile << ":\n";
        llvm::outs() << "  " << headerPath << "\n";
        llvm::outs() << "  " << sourcePath << "\n";
    }
}

void JzREHeaderTool::GenerateClassReflection(std::ostream &headerStream, std::ostream &sourceStream, const JzREHeaderToolReflectedClassInfo &classInfo)
{
    std::string className     = classInfo.name;
    std::string qualifiedName = classInfo.GetQualifiedName();

    sourceStream << "// Reflection for class: " << qualifiedName << "\n";

    // 生成类信息函数
    sourceStream << "const JzReflectedClassInfo& Get" << className << "ClassInfo() {\n";
    sourceStream << "    static JzReflectedClassInfo classInfo;\n";
    sourceStream << "    static bool initialized = false;\n";
    sourceStream << "    if (!initialized) {\n";
    sourceStream << "        classInfo.name = \"" << qualifiedName << "\";\n";
    sourceStream << "        classInfo.namespaceName = \"" << classInfo.namespaceName << "\";\n";
    sourceStream << "        classInfo.size = " << classInfo.sizeInBytes << ";\n";

    // 生成基类信息
    if (!classInfo.baseClasses.empty()) {
        for (const auto &baseClass : classInfo.baseClasses) {
            sourceStream << "        classInfo.baseClasses.push_back(\"" << baseClass << "\");\n";
        }
    }

    // 生成属性信息
    for (const auto &prop : classInfo.properties) {
        sourceStream << "\n        // Property: " << prop.name << "\n";
        sourceStream << "        {\n";
        sourceStream << "            JzReflectedClassPropertyInfo fieldInfo;\n";
        sourceStream << "            fieldInfo.name = \"" << prop.name << "\";\n";
        sourceStream << "            fieldInfo.type = \"" << SanitizeTypeName(prop.type) << "\";\n";
        sourceStream << "            fieldInfo.offset = " << prop.offset << ";\n";

        if (!prop.category.empty()) {
            sourceStream << "            fieldInfo.category = \"" << prop.category << "\";\n";
        }

        // 添加标志
        for (const auto &flag : prop.flags) {
            sourceStream << "            fieldInfo.flags.push_back(\"" << flag << "\");\n";
        }

        // 添加元数据
        for (const auto &meta : prop.metadata) {
            sourceStream << "            fieldInfo.metadata[\"" << meta.first
                         << "\"] = \"" << meta.second << "\";\n";
        }

        sourceStream << "            classInfo.properties.push_back(fieldInfo);\n";
        sourceStream << "        }\n";
    }

    // 生成方法信息
    for (const auto &method : classInfo.methods) {
        sourceStream << "\n        // Method: " << method.name << "\n";
        sourceStream << "        {\n";
        sourceStream << "            JzReflectedClassMethodInfo methodInfo;\n";
        sourceStream << "            methodInfo.name = \"" << method.name << "\";\n";
        sourceStream << "            methodInfo.returnType = \"" << SanitizeTypeName(method.returnType) << "\";\n";
        sourceStream << "            methodInfo.isConst = " << (method.isConst ? "true" : "false") << ";\n";
        sourceStream << "            methodInfo.isStatic = " << (method.isStatic ? "true" : "false") << ";\n";
        sourceStream << "            methodInfo.isVirtual = " << (method.isVirtual ? "true" : "false") << ";\n";

        // 生成参数信息
        for (const auto &param : method.parameters) {
            sourceStream << "            {\n";
            sourceStream << "                std::pair<String, String> paramInfo;\n";
            sourceStream << "                paramInfo.first = \"" << SanitizeTypeName(param.first) << "\";\n";
            sourceStream << "                paramInfo.second = \"" << param.second << "\";\n";
            sourceStream << "                methodInfo.parameters.push_back(paramInfo);\n";
            sourceStream << "            }\n";
        }

        // 添加元数据
        for (const auto &meta : method.metadata) {
            sourceStream << "            methodInfo.metadata[\"" << meta.first
                         << "\"] = \"" << meta.second << "\";\n";
        }

        sourceStream << "            classInfo.methods.push_back(methodInfo);\n";
        sourceStream << "        }\n";
    }

    // 设置类的大小
    sourceStream << "        classInfo.size = " << classInfo.sizeInBytes << ";\n";

    sourceStream << "        initialized = true;\n";
    sourceStream << "    }\n";
    sourceStream << "    return classInfo;\n";
    sourceStream << "}\n\n";
}

std::string JzREHeaderTool::SanitizeTypeName(const std::string &typeName)
{
    // 移除多余的空格和限定符
    std::string result = typeName;
    std::regex  pattern(R"(\s+)");
    result = std::regex_replace(result, pattern, " ");

    // 移除前后空格
    result.erase(0, result.find_first_not_of(" \t"));
    result.erase(result.find_last_not_of(" \t") + 1);

    return result;
}

std::string JzREHeaderTool::GetIncludeGuard(const std::string &fileName)
{
    std::string guard = fileName;
    std::transform(guard.begin(), guard.end(), guard.begin(), ::toupper);
    std::replace(guard.begin(), guard.end(), '.', '_');
    std::replace(guard.begin(), guard.end(), '/', '_');
    std::replace(guard.begin(), guard.end(), '\\', '_');
    return guard + "_GENERATED_H";
}

std::string JzREHeaderTool::GetRelativePath(const std::string &fullPath)
{
    if (JzREHeaderToolArgSource.empty()) {
        return fullPath;
    }

    // 简单实现：检查路径是否以SourceRoot开头，如果是则移除前缀
    llvm::StringRef fullPathRef(fullPath);
    llvm::StringRef sourceRootRef(JzREHeaderToolArgSource);

    if (fullPathRef.starts_with(sourceRootRef)) {
        llvm::StringRef relativePath = fullPathRef.drop_front(sourceRootRef.size());
        // 移除开头的路径分隔符
        while (relativePath.starts_with("/") || relativePath.starts_with("\\")) {
            relativePath = relativePath.drop_front(1);
        }
        return relativePath.str();
    }

    return fullPath;
}

std::unordered_map<std::string, std::string> JzREHeaderTool::ParseAnnotationString(const std::string &annotation)
{
    std::unordered_map<std::string, std::string> result;

    // 简单的键值对解析实现
    std::regex  pattern(R"((\w+)=([^,\)]+))");
    std::smatch match;
    std::string searchString = annotation;

    while (std::regex_search(searchString, match, pattern)) {
        if (match.size() == 3) {
            std::string key   = match[1].str();
            std::string value = match[2].str();
            if (value.front() == '"' && value.back() == '"') {
                value = value.substr(1, value.length() - 2);
            }
            result[key] = value;
        }
        searchString = match.suffix();
    }

    return result;
}

std::string JzREHeaderTool::GetFileBaseName(const std::string &filePath)
{
    // 获取文件名（不包含路径）
    size_t lastSlash = filePath.find_last_of("/\\");
    std::string fileName = (lastSlash == std::string::npos) ? filePath : filePath.substr(lastSlash + 1);
    
    // 移除文件扩展名
    size_t lastDot = fileName.find_last_of('.');
    if (lastDot != std::string::npos) {
        fileName = fileName.substr(0, lastDot);
    }
    
    return fileName;
}

void JzREHeaderTool::GenerateGeneratedHeaderFile(const std::string &headerFile, const std::vector<JzREHeaderToolReflectedClassInfo> &classes)
{
    if (classes.empty()) {
        return;
    }

    std::string fileBaseName = GetFileBaseName(headerFile);
    std::string generatedHeaderPath = JzREHeaderToolArgOutput + "/" + fileBaseName + ".generated.h";

    std::ofstream generatedFile(generatedHeaderPath);
    if (!generatedFile.is_open()) {
        llvm::errs() << "Error: Cannot create generated header file: " << generatedHeaderPath << "\n";
        return;
    }

    // 生成文件头部
    generatedFile << "// This file is automatically generated by JzREHeaderTool\n";
    generatedFile << "// DO NOT EDIT MANUALLY\n\n";
    generatedFile << "#pragma once\n\n";
    
    // 包含必要的头文件
    generatedFile << "#include \"JzReflectable.h\"\n";
    generatedFile << "#include \"JzReflectionRegistry.h\"\n\n";

    // 重新定义 GENERATED_BODY 宏以包含反射功能
    generatedFile << "// Redefine GENERATED_BODY macro to include reflection functionality\n";
    generatedFile << "#undef GENERATED_BODY\n";
    generatedFile << "#define GENERATED_BODY() \\\n";
    generatedFile << "public: \\\n";
    generatedFile << "    static const ::JzRE::JzReflectedClassInfo& GetStaticClass(); \\\n";
    generatedFile << "    virtual const ::JzRE::JzReflectedClassInfo& GetClass() const { return GetStaticClass(); } \\\n";
    generatedFile << "    static void RegisterReflection(); \\\n";
    generatedFile << "private:\n\n";

    // 生成函数声明
    generatedFile << "namespace JzRE {\n\n";
    for (const auto &classInfo : classes) {
        generatedFile << "// Class: " << classInfo.GetQualifiedName() << "\n";
        generatedFile << "const JzReflectedClassInfo& Get" << classInfo.name << "ClassInfo();\n";
        generatedFile << "void Register" << classInfo.name << "Reflection();\n\n";
    }
    generatedFile << "} // namespace JzRE\n";

    generatedFile.close();

    if (JzREHeaderToolArgVerbose) {
        llvm::outs() << "Generated header file: " << generatedHeaderPath << "\n";
    }
}

void JzREHeaderTool::GenerateGeneratedBodyMacro(std::ostream &headerStream, const JzREHeaderToolReflectedClassInfo &classInfo)
{
    headerStream << "// GENERATED_BODY macro content for " << classInfo.name << "\n";
    headerStream << "public:\n";
    headerStream << "    static const ::JzRE::JzReflectedClassInfo& GetStaticClass() {\n";
    headerStream << "        return ::JzRE::Get" << classInfo.name << "ClassInfo();\n";
    headerStream << "    }\n";
    headerStream << "    virtual const ::JzRE::JzReflectedClassInfo& GetClass() const override {\n";
    headerStream << "        return GetStaticClass();\n";
    headerStream << "    }\n";
    headerStream << "    static void RegisterReflection() {\n";
    headerStream << "        ::JzRE::Register" << classInfo.name << "Reflection();\n";
    headerStream << "    }\n";
}

bool JzREHeaderTool::NeedsGeneratedInclude(const std::string &headerFile)
{
    // 检查头文件是否包含反射标记的类
    for (const auto &classInfo : reflectedClasses) {
        if (classInfo.headerFile == headerFile) {
            return true;
        }
    }
    return false;
}