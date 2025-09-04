#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"

/**
 * @brief JzRE Tool Clang AST Visitor
 */
class JzREToolASTVisitor : public clang::RecursiveASTVisitor<JzREToolASTVisitor> {
public:
    bool VisitFunctionDecl(clang::FunctionDecl *FD)
    {
        llvm::outs() << "Found function: " << FD->getName() << "\n";
        return true; // 继续遍历
    }
};

/**
 * @brief JzRE Tool Clang AST Consumer
 */
class JzREToolASTConsumer : public clang::ASTConsumer {
public:
    void HandleTranslationUnit(clang::ASTContext &Context) override
    {
        Visitor.TraverseDecl(Context.getTranslationUnitDecl());
    }

private:
    JzREToolASTVisitor Visitor;
};

/**
 * @brief JzRE Tool Clang Frontend Action
 */
class JzREToolFrontendAction : public clang::ASTFrontendAction {
public:
    std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance &CI,
                                                          clang::StringRef         file) override
    {
        return std::make_unique<JzREToolASTConsumer>();
    }
};

/**
 * @brief JzRE Tool Clang Command Line Option
 */
static llvm::cl::OptionCategory JzREToolOptionCategory("JzRE Header Tool options");

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