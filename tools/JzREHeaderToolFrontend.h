#pragma once

#include "JzREHeaderToolASTConsumer.h"

#include <memory>

#include "clang/AST/ASTConsumer.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"

/**
 * @brief JzRE Tool Clang Frontend Action
 */
class JzREHeaderToolFrontendAction : public clang::ASTFrontendAction {
public:
    JzREHeaderToolFrontendAction(JzREHeaderTool *tool);

    std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance &ci, clang::StringRef file) override;

private:
    JzREHeaderTool *m_tool;
};

/**
 * @brief JzRE Tool Clang Frontend Action Factory
 */
class JzREHeaderToolFrontendActionFactory : public clang::tooling::FrontendActionFactory {
public:
    JzREHeaderToolFrontendActionFactory(JzREHeaderTool *tool);

    std::unique_ptr<clang::FrontendAction> create() override;

private:
    JzREHeaderTool *m_tool;
};
