#include <regex>
#include <unordered_set>
#include <fstream>

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/Attr.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/RecordLayout.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"

/**
 * @brief JzRE Tool Clang Command Line Option
 */
static llvm::cl::OptionCategory   JzREToolOptionCategory("JzRE Header Tool options");
static llvm::cl::extrahelp        JzREToolHelp(clang::tooling::CommonOptionsParser::HelpMessage);
static llvm::cl::opt<std::string> JzREToolArgSource("source",
                                                    llvm::cl::desc("Source directory"),
                                                    llvm::cl::value_desc("directory"),
                                                    llvm::cl::cat(JzREToolOptionCategory));
static llvm::cl::opt<std::string> JzREToolArgOutput("output",
                                                    llvm::cl::desc("Output directory for generated files"),
                                                    llvm::cl::value_desc("directory"),
                                                    llvm::cl::cat(JzREToolOptionCategory));
static llvm::cl::opt<bool>        JzREToolArgVerbose("verbose",
                                                     llvm::cl::desc("Enable verbose output"),
                                                     llvm::cl::cat(JzREToolOptionCategory));

/**
 * @brief JzRE Tool reflected class property info
 */
struct JzREToolReflectedClassPropertyInfo {
    std::string                                  name;
    std::string                                  type;
    std::string                                  category;
    std::vector<std::string>                     flags;
    std::unordered_map<std::string, std::string> metadata;
    size_t                                       offset;

    JzREToolReflectedClassPropertyInfo() :
        offset(0) { }
};

/**
 * @brief JzRE Tool reflected class method info
 */
struct JzREToolReflectedClassMethodInfo {
    std::string                                      name;
    std::string                                      returnType;
    std::vector<std::pair<std::string, std::string>> parameters; // type, name
    std::vector<std::string>                         flags;
    std::unordered_map<std::string, std::string>     metadata;
    bool                                             isConst;
    bool                                             isStatic;
    bool                                             isVirtual;

    JzREToolReflectedClassMethodInfo() :
        isConst(false), isStatic(false), isVirtual(false) { }
};

/**
 * @brief JzRE Tool reflected class info
 */
struct JzREToolReflectedClassInfo {
    std::string                                     name;
    std::string                                     namespaceName;
    std::string                                     fullName;
    std::string                                     headerFile;
    std::vector<std::string>                        baseClasses;
    std::vector<JzREToolReflectedClassPropertyInfo> properties;
    std::vector<JzREToolReflectedClassMethodInfo>   methods;
    std::unordered_map<std::string, std::string>    metadata;
    size_t                                          sizeInBytes;

    JzREToolReflectedClassInfo() :
        sizeInBytes(0) { }

    std::string GetQualifiedName() const
    {
        if (namespaceName.empty()) {
            return name;
        }
        return namespaceName + "::" + name;
    }
};

/**
 * @brief JzRE Tool Clang AST Visitor
 */
class JzREToolASTVisitor : public clang::RecursiveASTVisitor<JzREToolASTVisitor> {
public:
    explicit JzREToolASTVisitor(clang::ASTContext *context) :
        m_context(context) { }

    bool VisitCXXRecordDecl(clang::CXXRecordDecl *declaration)
    {
        if (!declaration->getDefinition()) {
            return true;
        }

        if (HasReflectedClassAttribute(declaration)) {
            ProcessReflectedClass(declaration);
        }

        return true;
    }

private:
    bool HasReflectedClassAttribute(const clang::CXXRecordDecl *classDecl)
    {
        for (const auto *Attr : classDecl->attrs()) {
            if (const auto *Annotate = dyn_cast<clang::AnnotateAttr>(Attr)) {
                std::string annotation = Annotate->getAnnotation().str();
                if (annotation.find("JzRE_CLASS") == 0) {
                    return true;
                }
            }
        }
        return false;
    }

    void ProcessReflectedClass(const clang::CXXRecordDecl *classDecl)
    {
        JzREToolReflectedClassInfo classInfo;

        // parse class baisc info
        classInfo.name     = classDecl->getNameAsString();
        classInfo.fullName = classDecl->getQualifiedNameAsString();

        // parse class namespace name
        if (const auto *DC = classDecl->getDeclContext()) {
            if (const auto *ND = dyn_cast<clang::NamespaceDecl>(DC)) {
                classInfo.namespaceName = ND->getQualifiedNameAsString();
            }
        }

        // parse class header file name
        clang::SourceManager &sm  = m_context->getSourceManager();
        clang::SourceLocation Loc = classDecl->getLocation();
        if (Loc.isValid()) {
            clang::FileID fid = sm.getFileID(Loc);
            if (auto fe = sm.getFileEntryRefForID(fid)) {
                classInfo.headerFile = std::string(fe->getName());
            }
        }

        // parse class size
        if (classDecl->isCompleteDefinition()) {
            const clang::ASTRecordLayout &layout = m_context->getASTRecordLayout(classDecl);
            classInfo.sizeInBytes                = layout.getSize().getQuantity();
        }

        // parse class annotation
        for (const auto *Attr : classDecl->attrs()) {
            if (const auto *Annotate = dyn_cast<clang::AnnotateAttr>(Attr)) {
                std::string annotation = Annotate->getAnnotation().str();
                if (annotation.find("JzRE_CLASS") == 0) {
                    auto metadata      = ParseReflectedAnnotation(annotation.substr(10)); // 移除 "JzRE_CLASS:"
                    classInfo.metadata = metadata;
                }
            }
        }

        // parse class base-class info
        for (const auto &base : classDecl->bases()) {
            clang::QualType baseType = base.getType();
            classInfo.baseClasses.push_back(baseType.getAsString());
        }

        // parse class property info
        for (const auto *field : classDecl->fields()) {
            if (HasReflectedPropertyAttribute(field)) {
                auto propInfo = ProcessReflectedProperty(field);
                classInfo.properties.push_back(propInfo);
            }
        }

        // parse class method info
        for (const auto *method : classDecl->methods()) {
            if (HasReflectedMethodAttribute(method)) {
                auto methodInfo = ProcessReflectedMethod(method);
                classInfo.methods.push_back(methodInfo);
            }
        }

        // Tool->AddReflectedClass(classInfo);

        if (JzREToolArgVerbose) {
            llvm::outs() << "Processed reflected class: " << classInfo.GetQualifiedName() << "\n";
            llvm::outs() << "  Properties: " << classInfo.properties.size() << "\n";
            llvm::outs() << "  Methods: " << classInfo.methods.size() << "\n";
        }
    }

    bool HasReflectedPropertyAttribute(const clang::FieldDecl *fieldDecl)
    {
        for (const auto *Attr : fieldDecl->attrs()) {
            if (const auto *Annotate = dyn_cast<clang::AnnotateAttr>(Attr)) {
                std::string annotation = Annotate->getAnnotation().str();
                if (annotation.find("JzRE_PROPERTY") == 0) {
                    return true;
                }
            }
        }
        return false;
    }

    JzREToolReflectedClassPropertyInfo ProcessReflectedProperty(const clang::FieldDecl *fieldDecl)
    {
        JzREToolReflectedClassPropertyInfo propInfo;

        // parse class property basic info
        propInfo.name = fieldDecl->getNameAsString();
        propInfo.type = fieldDecl->getType().getAsString();

        // parse class property offset
        if (const auto *Parent = dyn_cast<clang::RecordDecl>(fieldDecl->getDeclContext())) {
            const clang::ASTRecordLayout &layout  = m_context->getASTRecordLayout(Parent);
            unsigned                      fieldNo = fieldDecl->getFieldIndex();
            propInfo.offset                       = layout.getFieldOffset(fieldNo) / 8; // 转换为字节
        }

        // parse class property annotation
        for (const auto *Attr : fieldDecl->attrs()) {
            if (const auto *Annotate = dyn_cast<clang::AnnotateAttr>(Attr)) {
                std::string annotation = Annotate->getAnnotation().str();
                if (annotation.find("JzRE_PROPERTY") == 0) {
                    auto metadata     = ParseReflectedAnnotation(annotation.substr(13)); // 移除 "JzRE_PROPERTY:"
                    propInfo.metadata = metadata;

                    // parse flag
                    if (metadata.find("EditAnywhere") != metadata.end()) {
                        propInfo.flags.push_back("EditAnywhere");
                    }

                    if (metadata.find("Category") != metadata.end()) {
                        propInfo.category = metadata["Category"];
                    }
                }
            }
        }

        return propInfo;
    }

    bool HasReflectedMethodAttribute(const clang::CXXMethodDecl *methodDecl)
    {
        for (const auto *Attr : methodDecl->attrs()) {
            if (const auto *Annotate = dyn_cast<clang::AnnotateAttr>(Attr)) {
                std::string annotation = Annotate->getAnnotation().str();
                if (annotation.find("JzRE_METHOD") == 0) {
                    return true;
                }
            }
        }
        return false;
    }

    JzREToolReflectedClassMethodInfo ProcessReflectedMethod(const clang::CXXMethodDecl *methodDecl)
    {
        JzREToolReflectedClassMethodInfo methodInfo;

        // parse class method basic info
        methodInfo.name       = methodDecl->getNameAsString();
        methodInfo.returnType = methodDecl->getReturnType().getAsString();
        methodInfo.isConst    = methodDecl->isConst();
        methodInfo.isStatic   = methodDecl->isStatic();
        methodInfo.isVirtual  = methodDecl->isVirtual();

        // parse class method parameter
        for (const auto *Param : methodDecl->parameters()) {
            std::string paramType = Param->getType().getAsString();
            std::string paramName = Param->getNameAsString();
            methodInfo.parameters.emplace_back(paramType, paramName);
        }

        // parse class method annotation
        for (const auto *Attr : methodDecl->attrs()) {
            if (const auto *Annotate = dyn_cast<clang::AnnotateAttr>(Attr)) {
                std::string annotation = Annotate->getAnnotation().str();
                if (annotation.find("JzRE_METHOD") == 0) {
                    auto metadata       = ParseReflectedAnnotation(annotation.substr(11)); // 移除 "JzRE_METHOD:"
                    methodInfo.metadata = metadata;
                }
            }
        }

        return methodInfo;
    }

    std::unordered_map<std::string, std::string> ParseReflectedAnnotation(const std::string &annotation)
    {
        std::unordered_map<std::string, std::string> result;

        // parse simple key-value (key=value, key=value)
        std::regex  pattern(R"((\w+)=([^,\)]+))");
        std::smatch match;
        std::string searchString = annotation;

        while (std::regex_search(searchString, match, pattern)) {
            if (match.size() == 3) {
                std::string key   = match[1].str();
                std::string value = match[2].str();
                // 移除引号
                if (value.front() == '"' && value.back() == '"') {
                    value = value.substr(1, value.length() - 2);
                }
                result[key] = value;
            }
            searchString = match.suffix();
        }

        // parse simple flag
        std::regex flagPattern(R"(\b(\w+)\b)");
        searchString = annotation;
        while (std::regex_search(searchString, match, flagPattern)) {
            std::string flag = match[1].str();
            if (result.find(flag) == result.end()) {
                result[flag] = "true";
            }
            searchString = match.suffix();
        }

        return result;
    }

private:
    clang::ASTContext *m_context;
};

/**
 * @brief JzRE Tool Clang AST Consumer
 */
class JzREToolASTConsumer : public clang::ASTConsumer {
public:
    explicit JzREToolASTConsumer(clang::CompilerInstance *ci) :
        m_visitor(&(ci->getASTContext())) { }

    void HandleTranslationUnit(clang::ASTContext &context) override
    {
        m_visitor.TraverseDecl(context.getTranslationUnitDecl());
    }

private:
    JzREToolASTVisitor m_visitor;
};

/**
 * @brief JzRE Tool Clang Frontend Action
 */
class JzREToolFrontendAction : public clang::ASTFrontendAction {
public:
    std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance &ci, clang::StringRef file) override
    {
        return std::make_unique<JzREToolASTConsumer>(&ci);
    }
};

/**
 * @brief JzRE Tool Clang Frontend Action Factory
 */
class JzREToolFrontendActionFactory : public clang::tooling::FrontendActionFactory {
public:
    std::unique_ptr<clang::FrontendAction> create() override
    {
        return std::make_unique<JzREToolFrontendAction>();
    }
};

/**
 * @brief JzRE Header Tool
 */
class JzREHeaderTool {
public:
    JzREHeaderTool() = default;

    void ProcessFiles(const std::vector<std::string> &sourceFiles)
    {
        PreprocessPhase(sourceFiles);
        ParsePhase();
        ValidatePhase();
        GeneratePhase();
    }

    void PreprocessPhase(const std::vector<std::string> &sourceFiles)
    {
        if (JzREToolArgVerbose) {
            llvm::outs() << "=== Preprocessing Phase ===\n";
        }

        inputFiles = sourceFiles;

        // 收集所有需要处理的头文件
        for (const auto &file : sourceFiles) {
            if (llvm::sys::fs::exists(file)) {
                processedFiles.insert(file);
                if (JzREToolArgVerbose) {
                    llvm::outs() << "Added file: " << file << "\n";
                }
            } else {
                llvm::errs() << "Warning: File not found: " << file << "\n";
            }
        }

        if (JzREToolArgVerbose) {
            llvm::outs() << "Total files to process: " << processedFiles.size() << "\n";
        }
    }

    void ParsePhase()
    {
        if (JzREToolArgVerbose) {
            llvm::outs() << "=== Parsing Phase ===\n";
        }

        // 这个函数会被外部调用，通过Clang工具执行解析
    }

    void ValidatePhase()
    {
        if (JzREToolArgVerbose) {
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
                    if (JzREToolArgVerbose) {
                        llvm::outs() << "Info: Overloaded method in " << classInfo.name
                                     << ": " << pair.first << " (" << pair.second << " overloads)\n";
                    }
                }
            }
        }

        if (JzREToolArgVerbose) {
            llvm::outs() << "Validation completed. Found " << reflectedClasses.size()
                         << " reflected classes.\n";
        }
    }

    void GeneratePhase()
    {
        if (JzREToolArgVerbose) {
            llvm::outs() << "=== Generation Phase ===\n";
        }

        GenerateReflectionCode();
    }

    void GenerateReflectionCode()
    {
        if (JzREToolArgOutput.empty()) {
            llvm::errs() << "Error: Output directory not specified\n";
            return;
        }

        // 确保输出目录存在
        auto code = llvm::sys::fs::create_directories(JzREToolArgOutput);

        // TODO 每个文件生成一个
        std::string headerPath = JzREToolArgOutput + "/JzRE.generated.h";
        std::string sourcePath = JzREToolArgOutput + "/JzRE.generated.cpp";

        GenerateHeaderFile(headerPath);
        GenerateSourceFile(sourcePath);

        if (JzREToolArgVerbose) {
            llvm::outs() << "Generated files:\n";
            llvm::outs() << "  " << headerPath << "\n";
            llvm::outs() << "  " << sourcePath << "\n";
        }
    }

    void AddReflectedClass(const JzREToolReflectedClassInfo &classInfo)
    {
        reflectedClasses.push_back(classInfo);
    }

private:
    void GenerateHeaderFile(const std::string &outputPath)
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
            headerFile << "const JzReflectClassInfo& Get" << classInfo.name << "ClassInfo();\n\n";
        }

        headerFile << "} // namespace JzRE\n";
        headerFile.close();
    }

    void GenerateSourceFile(const std::string &outputPath)
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

    void GenerateClassReflection(std::ostream &headerStream, std::ostream &sourceStream, const JzREToolReflectedClassInfo &classInfo)
    {
        std::string className     = classInfo.name;
        std::string qualifiedName = classInfo.GetQualifiedName();

        sourceStream << "// Reflection for class: " << qualifiedName << "\n";

        // 生成类信息函数
        sourceStream << "const JzReflectClassInfo& Get" << className << "ClassInfo() {\n";
        sourceStream << "    static JzReflectClassInfo classInfo;\n";
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
            sourceStream << "            JzReflectFieldInfo fieldInfo;\n";
            sourceStream << "            fieldInfo.name = \"" << prop.name << "\";\n";
            sourceStream << "            fieldInfo.type = \"" << SanitizeTypeName(prop.type) << "\";\n";
            sourceStream << "            fieldInfo.offset = " << prop.offset << ";\n";
            sourceStream << "            fieldInfo.size = sizeof(" << SanitizeTypeName(prop.type) << ");\n";

            if (!prop.category.empty()) {
                sourceStream << "            fieldInfo.category = \"" << prop.category << "\";\n";
            }

            // 生成getter
            sourceStream << "            fieldInfo.getter = [](void* instance) -> std::any {\n";
            sourceStream << "                auto* obj = static_cast<" << qualifiedName << "*>(instance);\n";
            sourceStream << "                return obj->" << prop.name << ";\n";
            sourceStream << "            };\n";

            // 生成setter
            sourceStream << "            fieldInfo.setter = [](void* instance, const std::any& value) {\n";
            sourceStream << "                auto* obj = static_cast<" << qualifiedName << "*>(instance);\n";
            sourceStream << "                obj->" << prop.name << " = std::any_cast<"
                         << SanitizeTypeName(prop.type) << ">(value);\n";
            sourceStream << "            };\n";

            // 添加元数据
            for (const auto &meta : prop.metadata) {
                sourceStream << "            fieldInfo.metadata[\"" << meta.first
                             << "\"] = \"" << meta.second << "\";\n";
            }

            sourceStream << "            classInfo.fields[\"" << prop.name << "\"] = fieldInfo;\n";
            sourceStream << "        }\n";
        }

        // 生成方法信息
        for (const auto &method : classInfo.methods) {
            sourceStream << "\n        // Method: " << method.name << "\n";
            sourceStream << "        {\n";
            sourceStream << "            JzReflectMethodInfo methodInfo;\n";
            sourceStream << "            methodInfo.name = \"" << method.name << "\";\n";
            sourceStream << "            methodInfo.returnType = \"" << SanitizeTypeName(method.returnType) << "\";\n";
            sourceStream << "            methodInfo.isConst = " << (method.isConst ? "true" : "false") << ";\n";
            sourceStream << "            methodInfo.isStatic = " << (method.isStatic ? "true" : "false") << ";\n";
            sourceStream << "            methodInfo.isVirtual = " << (method.isVirtual ? "true" : "false") << ";\n";

            // 生成参数信息
            for (const auto &param : method.parameters) {
                sourceStream << "            {\n";
                sourceStream << "                JzReflectParameterInfo paramInfo;\n";
                sourceStream << "                paramInfo.name = \"" << param.second << "\";\n";
                sourceStream << "                paramInfo.type = \"" << SanitizeTypeName(param.first) << "\";\n";
                sourceStream << "                paramInfo.size = sizeof(" << SanitizeTypeName(param.first) << ");\n";
                sourceStream << "                methodInfo.parameters.push_back(paramInfo);\n";
                sourceStream << "            }\n";
            }

            // 添加元数据
            for (const auto &meta : method.metadata) {
                sourceStream << "            methodInfo.metadata[\"" << meta.first
                             << "\"] = \"" << meta.second << "\";\n";
            }

            sourceStream << "            classInfo.methods[\"" << method.name << "\"] = methodInfo;\n";
            sourceStream << "        }\n";
        }

        // 生成构造函数
        sourceStream << "\n        // Constructor\n";
        sourceStream << "        classInfo.createInstance = []() -> void* {\n";
        sourceStream << "            return new " << qualifiedName << "();\n";
        sourceStream << "        };\n";

        // 生成析构函数
        sourceStream << "        classInfo.destroyInstance = [](void* instance) {\n";
        sourceStream << "            delete static_cast<" << qualifiedName << "*>(instance);\n";
        sourceStream << "        };\n";

        sourceStream << "        initialized = true;\n";
        sourceStream << "    }\n";
        sourceStream << "    return classInfo;\n";
        sourceStream << "}\n\n";

        // 生成注册代码
        sourceStream << "namespace {\n";
        sourceStream << "    static bool " << className << "_registered = []() {\n";
        sourceStream << "        JzReflectionRegistry::GetInstance().RegisterClass(Get"
                     << className << "ClassInfo());\n";
        sourceStream << "        return true;\n";
        sourceStream << "    }();\n";
        sourceStream << "}\n\n";
    }

    // 工具函数
    std::string SanitizeTypeName(const std::string &typeName)
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

    std::string GetIncludeGuard(const std::string &fileName)
    {
        std::string guard = fileName;
        std::transform(guard.begin(), guard.end(), guard.begin(), ::toupper);
        std::replace(guard.begin(), guard.end(), '.', '_');
        std::replace(guard.begin(), guard.end(), '/', '_');
        std::replace(guard.begin(), guard.end(), '\\', '_');
        return guard + "_GENERATED_H";
    }

    std::string GetRelativePath(const std::string &fullPath)
    {
        if (JzREToolArgSource.empty()) {
            return fullPath;
        }

        // 简单实现：检查路径是否以SourceRoot开头，如果是则移除前缀
        llvm::StringRef fullPathRef(fullPath);
        llvm::StringRef sourceRootRef(JzREToolArgSource);

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

    // 解析注解字符串
    std::unordered_map<std::string, std::string> ParseAnnotationString(const std::string &annotation)
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

private:
    std::vector<std::string>                inputFiles;
    std::vector<JzREToolReflectedClassInfo> reflectedClasses;
    std::unordered_set<std::string>         processedFiles;
};

int main(int argc, const char **argv)
{
    auto expectedParser = clang::tooling::CommonOptionsParser::create(argc, argv, JzREToolOptionCategory);
    if (!expectedParser) {
        return 1;
    }

    auto                     &optionParser = expectedParser.get();
    clang::tooling::ClangTool Tool(optionParser.getCompilations(), optionParser.getSourcePathList());

    return Tool.run(clang::tooling::newFrontendActionFactory<JzREToolFrontendAction>().get());
}