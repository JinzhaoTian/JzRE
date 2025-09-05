#include <regex>

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

        // if (JzREToolArgVerbose) {
        //     llvm::outs() << "Processed reflected class: " << classInfo.GetQualifiedName() << "\n";
        //     llvm::outs() << "  Properties: " << classInfo.properties.size() << "\n";
        //     llvm::outs() << "  Methods: " << classInfo.methods.size() << "\n";
        // }
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
 * @brief JzRE Tool Clang Command Line Option
 */
static llvm::cl::OptionCategory   JzREToolOptionCategory("JzRE Header Tool options");
static llvm::cl::extrahelp        JzREToolHelp(clang::tooling::CommonOptionsParser::HelpMessage);
static llvm::cl::opt<std::string> JzREToolArgOutput("output",
                                                    llvm::cl::desc("Output directory for generated files"),
                                                    llvm::cl::value_desc("directory"),
                                                    llvm::cl::cat(JzREToolOptionCategory));
static llvm::cl::opt<std::string> JzREToolArgSource("source",
                                                    llvm::cl::desc("Source directory"),
                                                    llvm::cl::value_desc("directory"),
                                                    llvm::cl::cat(JzREToolOptionCategory));
static llvm::cl::opt<bool>        JzREToolArgVerbose("verbose",
                                                     llvm::cl::desc("Enable verbose output"),
                                                     llvm::cl::cat(JzREToolOptionCategory));

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