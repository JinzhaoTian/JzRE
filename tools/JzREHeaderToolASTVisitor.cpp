#include "JzREHeaderToolASTVisitor.h"
#include "JzREHeaderToolOptions.h"

#include <regex>

#include "clang/AST/Attr.h"
#include "clang/AST/RecordLayout.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Lex/Lexer.h"
#include "llvm/Support/raw_ostream.h"

JzREHeaderToolASTVisitor::JzREHeaderToolASTVisitor(clang::ASTContext *context, JzREHeaderTool *tool) :
    m_context(context), m_tool(tool) { }

bool JzREHeaderToolASTVisitor::VisitCXXRecordDecl(clang::CXXRecordDecl *declaration)
{
    if (!declaration->getDefinition()) {
        return true;
    }

    if (HasReflectedClassMacro(declaration)) {
        ProcessReflectedClass(declaration);
    }

    return true;
}

bool JzREHeaderToolASTVisitor::HasReflectedClassMacro(const clang::CXXRecordDecl *classDecl)
{
    clang::SourceManager &sm       = m_context->getSourceManager();
    clang::SourceLocation classLoc = classDecl->getLocation();

    if (!classLoc.isValid()) {
        return false;
    }

    // 获取类声明前的源代码行
    clang::SourceLocation lineStart = sm.translateLineCol(
        sm.getFileID(classLoc),
        sm.getSpellingLineNumber(classLoc) - 1, 1);

    if (!lineStart.isValid()) {
        return false;
    }

    // 读取前一行的源代码
    clang::SourceLocation lineEnd = clang::Lexer::getLocForEndOfToken(
        lineStart, 0, sm, m_context->getLangOpts());

    if (!lineEnd.isValid()) {
        lineEnd = classLoc;
    }

    bool            invalid  = false;
    llvm::StringRef lineText = clang::Lexer::getSourceText(
        clang::CharSourceRange::getCharRange(lineStart, lineEnd), sm, m_context->getLangOpts(), &invalid);

    if (invalid) {
        return false;
    }

    // 检查是否包含 JzRE_CLASS 宏
    return lineText.contains("JzRE_CLASS");
}

void JzREHeaderToolASTVisitor::ProcessReflectedClass(const clang::CXXRecordDecl *classDecl)
{
    JzREHeaderToolReflectedClassInfo classInfo;

    // parse class baisc info
    classInfo.name     = classDecl->getNameAsString();
    classInfo.fullName = classDecl->getQualifiedNameAsString();

    // parse class namespace name
    if (const auto *dc = classDecl->getDeclContext()) {
        if (const auto *nd = dyn_cast<clang::NamespaceDecl>(dc)) {
            classInfo.namespaceName = nd->getQualifiedNameAsString();
        }
    }

    // parse class header file name
    clang::SourceManager &sm  = m_context->getSourceManager();
    clang::SourceLocation loc = classDecl->getLocation();
    if (loc.isValid()) {
        clang::FileID fid = sm.getFileID(loc);
        if (auto fe = sm.getFileEntryRefForID(fid)) {
            classInfo.headerFile = std::string(fe->getName());
        }
    }

    // parse class size
    if (classDecl->isCompleteDefinition()) {
        const clang::ASTRecordLayout &layout = m_context->getASTRecordLayout(classDecl);
        classInfo.sizeInBytes                = layout.getSize().getQuantity();
    }

    // parse class macro arguments
    std::string macroArgs = ExtractMacroArgumentsFromSource(loc, "JzRE_CLASS");
    if (!macroArgs.empty()) {
        classInfo.metadata = ParseReflectedMacroArgs(macroArgs);
    }

    // parse class base-class info
    for (const auto &base : classDecl->bases()) {
        clang::QualType baseType = base.getType();
        classInfo.baseClasses.push_back(baseType.getAsString());
    }

    // parse class property info
    for (const auto *field : classDecl->fields()) {
        if (HasReflectedPropertyMacro(field)) {
            auto propInfo = ProcessReflectedProperty(field);
            classInfo.properties.push_back(propInfo);
        }
    }

    // parse class method info
    for (const auto *method : classDecl->methods()) {
        if (HasReflectedMethodMacro(method)) {
            auto methodInfo = ProcessReflectedMethod(method);
            classInfo.methods.push_back(methodInfo);
        }
    }

    m_tool->AddReflectedClass(classInfo);

    if (JzREHeaderToolArgVerbose) {
        llvm::outs() << "Processed reflected class: " << classInfo.GetQualifiedName() << "\n";
        llvm::outs() << "  Properties: " << classInfo.properties.size() << "\n";
        llvm::outs() << "  Methods: " << classInfo.methods.size() << "\n";
    }
}

bool JzREHeaderToolASTVisitor::HasReflectedPropertyMacro(const clang::FieldDecl *fieldDecl)
{
    clang::SourceManager &sm       = m_context->getSourceManager();
    clang::SourceLocation fieldLoc = fieldDecl->getLocation();

    if (!fieldLoc.isValid()) {
        return false;
    }

    // 获取字段声明前的源代码行
    clang::SourceLocation lineStart = sm.translateLineCol(
        sm.getFileID(fieldLoc),
        sm.getSpellingLineNumber(fieldLoc) - 1, 1);

    if (!lineStart.isValid()) {
        return false;
    }

    // 读取前一行的源代码
    clang::SourceLocation lineEnd = fieldLoc;

    bool            invalid  = false;
    llvm::StringRef lineText = clang::Lexer::getSourceText(
        clang::CharSourceRange::getCharRange(lineStart, lineEnd), sm, m_context->getLangOpts(), &invalid);

    if (invalid) {
        return false;
    }

    // 检查是否包含 JzRE_PROPERTY 宏
    return lineText.contains("JzRE_PROPERTY");
}

JzREHeaderToolReflectedClassPropertyInfo JzREHeaderToolASTVisitor::ProcessReflectedProperty(const clang::FieldDecl *fieldDecl)
{
    JzREHeaderToolReflectedClassPropertyInfo propInfo;

    // parse class property basic info
    propInfo.name = fieldDecl->getNameAsString();
    propInfo.type = fieldDecl->getType().getAsString();

    // parse class property offset
    if (const auto *parent = dyn_cast<clang::RecordDecl>(fieldDecl->getDeclContext())) {
        const clang::ASTRecordLayout &layout  = m_context->getASTRecordLayout(parent);
        unsigned                      fieldNo = fieldDecl->getFieldIndex();
        propInfo.offset                       = layout.getFieldOffset(fieldNo) / 8; // 转换为字节
    }

    // parse class property macro arguments
    clang::SourceManager &sm        = m_context->getSourceManager();
    clang::SourceLocation fieldLoc  = fieldDecl->getLocation();
    std::string           macroArgs = ExtractMacroArgumentsFromSource(fieldLoc, "JzRE_PROPERTY");
    if (!macroArgs.empty()) {
        propInfo.metadata = ParseReflectedMacroArgs(macroArgs);

        // parse flag
        if (propInfo.metadata.find("EditAnywhere") != propInfo.metadata.end()) {
            propInfo.flags.push_back("EditAnywhere");
        }

        if (propInfo.metadata.find("Category") != propInfo.metadata.end()) {
            propInfo.category = propInfo.metadata["Category"];
        }
    }

    return propInfo;
}

bool JzREHeaderToolASTVisitor::HasReflectedMethodMacro(const clang::CXXMethodDecl *methodDecl)
{
    clang::SourceManager &sm        = m_context->getSourceManager();
    clang::SourceLocation methodLoc = methodDecl->getLocation();

    if (!methodLoc.isValid()) {
        return false;
    }

    // 获取方法声明前的源代码行
    clang::SourceLocation lineStart = sm.translateLineCol(
        sm.getFileID(methodLoc),
        sm.getSpellingLineNumber(methodLoc) - 1, 1);

    if (!lineStart.isValid()) {
        return false;
    }

    // 读取前一行的源代码
    clang::SourceLocation lineEnd = methodLoc;

    bool            invalid  = false;
    llvm::StringRef lineText = clang::Lexer::getSourceText(
        clang::CharSourceRange::getCharRange(lineStart, lineEnd), sm, m_context->getLangOpts(), &invalid);

    if (invalid) {
        return false;
    }

    // 检查是否包含 JzRE_METHOD 宏
    return lineText.contains("JzRE_METHOD");
}

JzREHeaderToolReflectedClassMethodInfo JzREHeaderToolASTVisitor::ProcessReflectedMethod(const clang::CXXMethodDecl *methodDecl)
{
    JzREHeaderToolReflectedClassMethodInfo methodInfo;

    // parse class method basic info
    methodInfo.name       = methodDecl->getNameAsString();
    methodInfo.returnType = methodDecl->getReturnType().getAsString();
    methodInfo.isConst    = methodDecl->isConst();
    methodInfo.isStatic   = methodDecl->isStatic();
    methodInfo.isVirtual  = methodDecl->isVirtual();

    // parse class method parameter
    for (const auto *param : methodDecl->parameters()) {
        std::string paramType = param->getType().getAsString();
        std::string paramName = param->getNameAsString();
        methodInfo.parameters.emplace_back(paramType, paramName);
    }

    // parse class method macro arguments
    clang::SourceManager &sm        = m_context->getSourceManager();
    clang::SourceLocation methodLoc = methodDecl->getLocation();
    std::string           macroArgs = ExtractMacroArgumentsFromSource(methodLoc, "JzRE_METHOD");
    if (!macroArgs.empty()) {
        methodInfo.metadata = ParseReflectedMacroArgs(macroArgs);
    }

    return methodInfo;
}

std::unordered_map<std::string, std::string> JzREHeaderToolASTVisitor::ParseReflectedMacroArgs(const std::string &macroArgs)
{
    std::unordered_map<std::string, std::string> result;

    if (macroArgs.empty()) {
        return result;
    }

    // parse simple key-value (key=value, key=value)
    std::regex  pattern(R"((\w+)=([^,\)]+))");
    std::smatch match;
    std::string searchString = macroArgs;

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
    searchString = macroArgs;
    while (std::regex_search(searchString, match, flagPattern)) {
        std::string flag = match[1].str();
        if (result.find(flag) == result.end()) {
            result[flag] = "true";
        }
        searchString = match.suffix();
    }

    return result;
}

std::string JzREHeaderToolASTVisitor::ExtractMacroArgumentsFromSource(clang::SourceLocation startLoc, const std::string &macroName)
{
    clang::SourceManager &sm = m_context->getSourceManager();

    if (!startLoc.isValid()) {
        return "";
    }

    // 获取宏调用前的源代码行
    clang::SourceLocation lineStart = sm.translateLineCol(
        sm.getFileID(startLoc),
        sm.getSpellingLineNumber(startLoc) - 1, 1);

    if (!lineStart.isValid()) {
        return "";
    }

    // 读取整行源代码
    clang::SourceLocation lineEnd = clang::Lexer::getLocForEndOfToken(
        sm.translateLineCol(sm.getFileID(startLoc), sm.getSpellingLineNumber(startLoc) - 1, 200),
        0, sm, m_context->getLangOpts());

    bool            invalid  = false;
    llvm::StringRef lineText = clang::Lexer::getSourceText(
        clang::CharSourceRange::getCharRange(lineStart, lineEnd), sm, m_context->getLangOpts(), &invalid);

    if (invalid) {
        return "";
    }

    std::string line = lineText.str();

    // 查找宏调用
    size_t macroPos = line.find(macroName);
    if (macroPos == std::string::npos) {
        return "";
    }

    // 查找括号
    size_t openParen = line.find('(', macroPos);
    if (openParen == std::string::npos) {
        return ""; // 无参数宏
    }

    size_t closeParen = line.find(')', openParen);
    if (closeParen == std::string::npos) {
        return "";
    }

    // 提取括号内的内容
    return line.substr(openParen + 1, closeParen - openParen - 1);
}
