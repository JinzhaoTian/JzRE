#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/AST/Attr.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/RecordLayout.h"
#include "clang/Basic/SourceManager.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/FileSystem.h"
#include <fstream>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <string>
#include <regex>

using namespace clang;
using namespace clang::tooling;

// 命令行选项
static llvm::cl::OptionCategory   MyToolCategory("JzRE Header Tool Options");
static llvm::cl::extrahelp        CommonHelp(CommonOptionsParser::HelpMessage);
static llvm::cl::opt<std::string> OutputDir("output-dir", llvm::cl::desc("Output directory for generated files"),
                                            llvm::cl::value_desc("directory"), llvm::cl::cat(MyToolCategory));
static llvm::cl::opt<std::string> SourceRoot("source-root", llvm::cl::desc("Source root directory"),
                                             llvm::cl::value_desc("directory"), llvm::cl::cat(MyToolCategory));
static llvm::cl::opt<bool>        Verbose("verbose", llvm::cl::desc("Enable verbose output"),
                                          llvm::cl::cat(MyToolCategory));

// 反射属性信息
struct ReflectPropertyInfo {
    std::string                                  name;
    std::string                                  type;
    std::string                                  category;
    std::vector<std::string>                     flags;
    std::unordered_map<std::string, std::string> metadata;
    size_t                                       offset;

    ReflectPropertyInfo() :
        offset(0) { }
};

// 反射方法信息
struct ReflectMethodInfo {
    std::string                                      name;
    std::string                                      returnType;
    std::vector<std::pair<std::string, std::string>> parameters; // type, name
    std::vector<std::string>                         flags;
    std::unordered_map<std::string, std::string>     metadata;
    bool                                             isConst;
    bool                                             isStatic;
    bool                                             isVirtual;

    ReflectMethodInfo() :
        isConst(false), isStatic(false), isVirtual(false) { }
};

// 反射类信息
struct ReflectClassInfo {
    std::string                                  name;
    std::string                                  namespaceName;
    std::string                                  fullName;
    std::string                                  headerFile;
    std::vector<std::string>                     baseClasses;
    std::vector<ReflectPropertyInfo>             properties;
    std::vector<ReflectMethodInfo>               methods;
    std::unordered_map<std::string, std::string> metadata;
    size_t                                       sizeInBytes;

    ReflectClassInfo() :
        sizeInBytes(0) { }

    std::string GetQualifiedName() const
    {
        if (namespaceName.empty()) {
            return name;
        }
        return namespaceName + "::" + name;
    }
};

// 前向声明
class JzREHeaderTool;

// AST访问器
class ReflectionASTVisitor : public RecursiveASTVisitor<ReflectionASTVisitor> {
public:
    explicit ReflectionASTVisitor(ASTContext *Context, JzREHeaderTool *Tool);
    bool VisitCXXRecordDecl(CXXRecordDecl *Declaration);

private:
    ASTContext     *Context;
    JzREHeaderTool *Tool;

    bool                                         HasReflectionAttribute(const CXXRecordDecl *Decl);
    void                                         ProcessReflectedClass(const CXXRecordDecl *ClassDecl);
    bool                                         HasPropertyAttribute(const FieldDecl *Field);
    bool                                         HasFunctionAttribute(const CXXMethodDecl *Method);
    ReflectPropertyInfo                          ProcessProperty(const FieldDecl *Field);
    ReflectMethodInfo                            ProcessMethod(const CXXMethodDecl *Method);
    std::unordered_map<std::string, std::string> ParseAnnotationString(const std::string &annotation);
};

// 工具主类
class JzREHeaderTool {
public:
    JzREHeaderTool() = default;

    void ProcessFiles(const std::vector<std::string> &sourceFiles);
    void GenerateReflectionCode();
    void AddReflectedClass(const ReflectClassInfo &classInfo);

    // 阶段处理函数
    void PreprocessPhase(const std::vector<std::string> &sourceFiles);
    void ParsePhase();
    void ValidatePhase();
    void GeneratePhase();

private:
    std::vector<std::string>        inputFiles;
    std::vector<ReflectClassInfo>   reflectedClasses;
    std::unordered_set<std::string> processedFiles;

    // 解析注解字符串
    std::unordered_map<std::string, std::string> ParseAnnotationString(const std::string &annotation);

    // 代码生成函数
    void GenerateHeaderFile(const std::string &outputPath);
    void GenerateSourceFile(const std::string &outputPath);
    void GenerateClassReflection(std::ostream &headerStream, std::ostream &sourceStream,
                                 const ReflectClassInfo &classInfo);

    // 工具函数
    std::string SanitizeTypeName(const std::string &typeName);
    std::string GetIncludeGuard(const std::string &fileName);
    std::string GetRelativePath(const std::string &fullPath);
};

// AST访问器实现
ReflectionASTVisitor::ReflectionASTVisitor(ASTContext *Context, JzREHeaderTool *Tool) :
    Context(Context), Tool(Tool) { }

bool ReflectionASTVisitor::VisitCXXRecordDecl(CXXRecordDecl *Declaration)
{
    if (!Declaration->getDefinition()) {
        return true;
    }

    if (HasReflectionAttribute(Declaration)) {
        ProcessReflectedClass(Declaration);
    }

    return true;
}

bool ReflectionASTVisitor::HasReflectionAttribute(const CXXRecordDecl *Decl)
{
    for (const auto *Attr : Decl->attrs()) {
        if (const auto *Annotate = dyn_cast<AnnotateAttr>(Attr)) {
            std::string annotation = Annotate->getAnnotation().str();
            if (annotation.find("JzRE_CLASS") == 0) {
                return true;
            }
        }
    }
    return false;
}

void ReflectionASTVisitor::ProcessReflectedClass(const CXXRecordDecl *ClassDecl)
{
    ReflectClassInfo classInfo;

    // 基本类信息
    classInfo.name     = ClassDecl->getNameAsString();
    classInfo.fullName = ClassDecl->getQualifiedNameAsString();

    // 命名空间
    if (const auto *DC = ClassDecl->getDeclContext()) {
        if (const auto *ND = dyn_cast<NamespaceDecl>(DC)) {
            classInfo.namespaceName = ND->getQualifiedNameAsString();
        }
    }

    // 头文件信息
    SourceManager &SM  = Context->getSourceManager();
    SourceLocation Loc = ClassDecl->getLocation();
    if (Loc.isValid()) {
        FileID FID = SM.getFileID(Loc);
        if (auto FE = SM.getFileEntryRefForID(FID)) {
            classInfo.headerFile = std::string(FE->getName());
        }
    }

    // 类大小
    if (ClassDecl->isCompleteDefinition()) {
        const ASTRecordLayout &Layout = Context->getASTRecordLayout(ClassDecl);
        classInfo.sizeInBytes         = Layout.getSize().getQuantity();
    }

    // 解析类注解
    for (const auto *Attr : ClassDecl->attrs()) {
        if (const auto *Annotate = dyn_cast<AnnotateAttr>(Attr)) {
            std::string annotation = Annotate->getAnnotation().str();
            if (annotation.find("JzRE_CLASS") == 0) {
                auto metadata      = ParseAnnotationString(annotation.substr(10)); // 移除 "JzRE_CLASS:"
                classInfo.metadata = metadata;
            }
        }
    }

    // 处理基类
    for (const auto &Base : ClassDecl->bases()) {
        QualType BaseType = Base.getType();
        classInfo.baseClasses.push_back(BaseType.getAsString());
    }

    // 处理字段
    for (const auto *Field : ClassDecl->fields()) {
        if (HasPropertyAttribute(Field)) {
            ReflectPropertyInfo propInfo = ProcessProperty(Field);
            classInfo.properties.push_back(propInfo);
        }
    }

    // 处理方法
    for (const auto *Method : ClassDecl->methods()) {
        if (HasFunctionAttribute(Method)) {
            ReflectMethodInfo methodInfo = ProcessMethod(Method);
            classInfo.methods.push_back(methodInfo);
        }
    }

    // 将类信息添加到工具中
    Tool->AddReflectedClass(classInfo);

    if (Verbose) {
        llvm::outs() << "Processed reflected class: " << classInfo.GetQualifiedName() << "\n";
        llvm::outs() << "  Properties: " << classInfo.properties.size() << "\n";
        llvm::outs() << "  Methods: " << classInfo.methods.size() << "\n";
    }
}

bool ReflectionASTVisitor::HasPropertyAttribute(const FieldDecl *Field)
{
    for (const auto *Attr : Field->attrs()) {
        if (const auto *Annotate = dyn_cast<AnnotateAttr>(Attr)) {
            std::string annotation = Annotate->getAnnotation().str();
            if (annotation.find("JzRE_PROPERTY") == 0) {
                return true;
            }
        }
    }
    return false;
}

bool ReflectionASTVisitor::HasFunctionAttribute(const CXXMethodDecl *Method)
{
    for (const auto *Attr : Method->attrs()) {
        if (const auto *Annotate = dyn_cast<AnnotateAttr>(Attr)) {
            std::string annotation = Annotate->getAnnotation().str();
            if (annotation.find("JzRE_FUNCTION") == 0) {
                return true;
            }
        }
    }
    return false;
}

ReflectPropertyInfo ReflectionASTVisitor::ProcessProperty(const FieldDecl *Field)
{
    ReflectPropertyInfo propInfo;

    propInfo.name = Field->getNameAsString();
    propInfo.type = Field->getType().getAsString();

    // 计算偏移量
    if (const auto *Parent = dyn_cast<RecordDecl>(Field->getDeclContext())) {
        const ASTRecordLayout &Layout  = Context->getASTRecordLayout(Parent);
        unsigned               FieldNo = Field->getFieldIndex();
        propInfo.offset                = Layout.getFieldOffset(FieldNo) / 8; // 转换为字节
    }

    // 解析属性注解
    for (const auto *Attr : Field->attrs()) {
        if (const auto *Annotate = dyn_cast<AnnotateAttr>(Attr)) {
            std::string annotation = Annotate->getAnnotation().str();
            if (annotation.find("JzRE_PROPERTY") == 0) {
                auto metadata     = ParseAnnotationString(annotation.substr(13)); // 移除 "JzRE_PROPERTY:"
                propInfo.metadata = metadata;

                // 解析特殊标志
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

ReflectMethodInfo ReflectionASTVisitor::ProcessMethod(const CXXMethodDecl *Method)
{
    ReflectMethodInfo methodInfo;

    methodInfo.name       = Method->getNameAsString();
    methodInfo.returnType = Method->getReturnType().getAsString();
    methodInfo.isConst    = Method->isConst();
    methodInfo.isStatic   = Method->isStatic();
    methodInfo.isVirtual  = Method->isVirtual();

    // 处理参数
    for (const auto *Param : Method->parameters()) {
        std::string paramType = Param->getType().getAsString();
        std::string paramName = Param->getNameAsString();
        methodInfo.parameters.emplace_back(paramType, paramName);
    }

    // 解析方法注解
    for (const auto *Attr : Method->attrs()) {
        if (const auto *Annotate = dyn_cast<AnnotateAttr>(Attr)) {
            std::string annotation = Annotate->getAnnotation().str();
            if (annotation.find("JzRE_FUNCTION") == 0) {
                auto metadata       = ParseAnnotationString(annotation.substr(13)); // 移除 "JzRE_FUNCTION:"
                methodInfo.metadata = metadata;
            }
        }
    }

    return methodInfo;
}

std::unordered_map<std::string, std::string> ReflectionASTVisitor::ParseAnnotationString(const std::string &annotation)
{
    std::unordered_map<std::string, std::string> result;

    // 简单的键值对解析 (key=value, key=value)
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

    // 处理简单标志（不带值的）
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

// AST Consumer
class ReflectionASTConsumer : public ASTConsumer {
public:
    explicit ReflectionASTConsumer(CompilerInstance *CI, JzREHeaderTool *Tool) :
        Visitor(&(CI->getASTContext()), Tool) { }

    void HandleTranslationUnit(ASTContext &Context) override
    {
        Visitor.TraverseDecl(Context.getTranslationUnitDecl());
    }

private:
    ReflectionASTVisitor Visitor;
};

// Frontend Action
class ReflectionFrontendAction : public ASTFrontendAction {
public:
    ReflectionFrontendAction(JzREHeaderTool *Tool) :
        Tool(Tool) { }

    std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI, StringRef file) override
    {
        return std::make_unique<ReflectionASTConsumer>(&CI, Tool);
    }

private:
    JzREHeaderTool *Tool;
};

// Frontend Action Factory
class ReflectionFrontendActionFactory : public FrontendActionFactory {
public:
    ReflectionFrontendActionFactory(JzREHeaderTool *Tool) :
        Tool(Tool) { }

    std::unique_ptr<FrontendAction> create() override
    {
        return std::make_unique<ReflectionFrontendAction>(Tool);
    }

private:
    JzREHeaderTool *Tool;
};

// JzREHeaderTool 实现
void JzREHeaderTool::ProcessFiles(const std::vector<std::string> &sourceFiles)
{
    PreprocessPhase(sourceFiles);
    ParsePhase();
    ValidatePhase();
    GeneratePhase();
}

void JzREHeaderTool::PreprocessPhase(const std::vector<std::string> &sourceFiles)
{
    if (Verbose) {
        llvm::outs() << "=== Preprocessing Phase ===\n";
    }

    inputFiles = sourceFiles;

    // 收集所有需要处理的头文件
    for (const auto &file : sourceFiles) {
        if (llvm::sys::fs::exists(file)) {
            processedFiles.insert(file);
            if (Verbose) {
                llvm::outs() << "Added file: " << file << "\n";
            }
        } else {
            llvm::errs() << "Warning: File not found: " << file << "\n";
        }
    }

    if (Verbose) {
        llvm::outs() << "Total files to process: " << processedFiles.size() << "\n";
    }
}

void JzREHeaderTool::ParsePhase()
{
    if (Verbose) {
        llvm::outs() << "=== Parsing Phase ===\n";
    }

    // 这个函数会被外部调用，通过Clang工具执行解析
}

void JzREHeaderTool::ValidatePhase()
{
    if (Verbose) {
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
                if (Verbose) {
                    llvm::outs() << "Info: Overloaded method in " << classInfo.name
                                 << ": " << pair.first << " (" << pair.second << " overloads)\n";
                }
            }
        }
    }

    if (Verbose) {
        llvm::outs() << "Validation completed. Found " << reflectedClasses.size()
                     << " reflected classes.\n";
    }
}

void JzREHeaderTool::GeneratePhase()
{
    if (Verbose) {
        llvm::outs() << "=== Generation Phase ===\n";
    }

    GenerateReflectionCode();
}

void JzREHeaderTool::AddReflectedClass(const ReflectClassInfo &classInfo)
{
    reflectedClasses.push_back(classInfo);
}

void JzREHeaderTool::GenerateReflectionCode()
{
    if (OutputDir.empty()) {
        llvm::errs() << "Error: Output directory not specified\n";
        return;
    }

    // 确保输出目录存在
    llvm::sys::fs::create_directories(OutputDir);

    std::string headerPath = OutputDir + "/JzRE.generated.h";
    std::string sourcePath = OutputDir + "/JzRE.generated.cpp";

    GenerateHeaderFile(headerPath);
    GenerateSourceFile(sourcePath);

    if (Verbose) {
        llvm::outs() << "Generated files:\n";
        llvm::outs() << "  " << headerPath << "\n";
        llvm::outs() << "  " << sourcePath << "\n";
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
        headerFile << "const JzReflectClassInfo& Get" << classInfo.name << "ClassInfo();\n\n";
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

void JzREHeaderTool::GenerateClassReflection(std::ostream &headerStream, std::ostream &sourceStream,
                                             const ReflectClassInfo &classInfo)
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
    if (SourceRoot.empty()) {
        return fullPath;
    }

    // 简单实现：检查路径是否以SourceRoot开头，如果是则移除前缀
    llvm::StringRef fullPathRef(fullPath);
    llvm::StringRef sourceRootRef(SourceRoot);
    
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

// 主函数
int main(int argc, const char **argv)
{
    auto ExpectedParser = CommonOptionsParser::create(argc, argv, MyToolCategory);
    if (!ExpectedParser) {
        llvm::errs() << "Error creating CommonOptionsParser: " << llvm::toString(ExpectedParser.takeError()) << "\n";
        return 1;
    }
    
    CommonOptionsParser &OptionsParser = ExpectedParser.get();
    ClangTool           Tool(OptionsParser.getCompilations(), OptionsParser.getSourcePathList());

    // 创建工具实例
    JzREHeaderTool headerTool;

    // 预处理阶段
    headerTool.PreprocessPhase(OptionsParser.getSourcePathList());

    // 运行Clang工具进行解析
    ReflectionFrontendActionFactory Factory(&headerTool);
    int                             result = Tool.run(&Factory);

    if (result == 0) {
        // 验证和生成阶段
        headerTool.ValidatePhase();
        headerTool.GeneratePhase();
    }

    return result;
}
