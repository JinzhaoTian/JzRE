#include "JzREHeaderToolFrontend.h"
#include "JzREHeaderTool.h"

JzREHeaderToolFrontendAction::JzREHeaderToolFrontendAction(JzREHeaderTool *tool) :
    m_tool(tool) { }

std::unique_ptr<clang::ASTConsumer> JzREHeaderToolFrontendAction::CreateASTConsumer(clang::CompilerInstance &ci, clang::StringRef file)
{
    return std::make_unique<JzREHeaderToolASTConsumer>(&ci, m_tool);
}

JzREHeaderToolFrontendActionFactory::JzREHeaderToolFrontendActionFactory(JzREHeaderTool *tool) :
    m_tool(tool) { }

std::unique_ptr<clang::FrontendAction> JzREHeaderToolFrontendActionFactory::create()
{
    return std::make_unique<JzREHeaderToolFrontendAction>(m_tool);
}
