#include "JzREHeaderToolASTVisitor.h"
#include "JzREHeaderToolOptions.h"

#include <regex>

#include "clang/AST/Attr.h"
#include "clang/AST/RecordLayout.h"
#include <clang/Basic/SourceManager.h>
#include "llvm/Support/raw_ostream.h"

JzREHeaderToolASTVisitor::JzREHeaderToolASTVisitor(clang::ASTContext *context, JzREHeaderTool *tool) :
    m_context(context), m_tool(tool) { }

bool JzREHeaderToolASTVisitor::VisitCXXRecordDecl(clang::CXXRecordDecl *declaration)
{
    if (!declaration->getDefinition()) {
        return true;
    }

    if (HasReflectedClassAttribute(declaration)) {
        ProcessReflectedClass(declaration);
    }

    return true;
}

bool JzREHeaderToolASTVisitor::HasReflectedClassAttribute(const clang::CXXRecordDecl *classDecl)
{
    for (const auto *attr : classDecl->attrs()) {
        if (const auto *annotate = dyn_cast<clang::AnnotateAttr>(attr)) {
            std::string annotation = annotate->getAnnotation().str();
            if (annotation.find("JzRE_CLASS") == 0) {
                return true;
            }
        }
    }
    return false;
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

    // parse class annotation
    for (const auto *attr : classDecl->attrs()) {
        if (const auto *annotate = dyn_cast<clang::AnnotateAttr>(attr)) {
            std::string annotation = annotate->getAnnotation().str();
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

    m_tool->AddReflectedClass(classInfo);

    if (JzREHeaderToolArgVerbose) {
        llvm::outs() << "Processed reflected class: " << classInfo.GetQualifiedName() << "\n";
        llvm::outs() << "  Properties: " << classInfo.properties.size() << "\n";
        llvm::outs() << "  Methods: " << classInfo.methods.size() << "\n";
    }
}

bool JzREHeaderToolASTVisitor::HasReflectedPropertyAttribute(const clang::FieldDecl *fieldDecl)
{
    for (const auto *attr : fieldDecl->attrs()) {
        if (const auto *annotate = dyn_cast<clang::AnnotateAttr>(attr)) {
            std::string annotation = annotate->getAnnotation().str();
            if (annotation.find("JzRE_PROPERTY") == 0) {
                return true;
            }
        }
    }
    return false;
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

    // parse class property annotation
    for (const auto *attr : fieldDecl->attrs()) {
        if (const auto *annotate = dyn_cast<clang::AnnotateAttr>(attr)) {
            std::string annotation = annotate->getAnnotation().str();
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

bool JzREHeaderToolASTVisitor::HasReflectedMethodAttribute(const clang::CXXMethodDecl *methodDecl)
{
    for (const auto *attr : methodDecl->attrs()) {
        if (const auto *annotate = dyn_cast<clang::AnnotateAttr>(attr)) {
            std::string annotation = annotate->getAnnotation().str();
            if (annotation.find("JzRE_METHOD") == 0) {
                return true;
            }
        }
    }
    return false;
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

    // parse class method annotation
    for (const auto *attr : methodDecl->attrs()) {
        if (const auto *annotate = dyn_cast<clang::AnnotateAttr>(attr)) {
            std::string annotation = annotate->getAnnotation().str();
            if (annotation.find("JzRE_METHOD") == 0) {
                auto metadata       = ParseReflectedAnnotation(annotation.substr(11)); // 移除 "JzRE_METHOD:"
                methodInfo.metadata = metadata;
            }
        }
    }

    return methodInfo;
}

std::unordered_map<std::string, std::string> JzREHeaderToolASTVisitor::ParseReflectedAnnotation(const std::string &annotation)
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
