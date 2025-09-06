#pragma once

#include "JzREHeaderToolASTVisitor.h"

#include "clang/AST/ASTConsumer.h"
#include "clang/Frontend/CompilerInstance.h"

/**
 * @brief JzRE Tool Clang AST Consumer
 */
class JzREHeaderToolASTConsumer : public clang::ASTConsumer {
public:
    explicit JzREHeaderToolASTConsumer(clang::CompilerInstance *ci, JzREHeaderTool *tool);

    void HandleTranslationUnit(clang::ASTContext &context) override;

private:
    JzREHeaderToolASTVisitor m_visitor;
};
