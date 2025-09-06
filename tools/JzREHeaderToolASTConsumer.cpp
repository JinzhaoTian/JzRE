#include "JzREHeaderToolASTConsumer.h"
#include "JzREHeaderTool.h"

JzREHeaderToolASTConsumer::JzREHeaderToolASTConsumer(clang::CompilerInstance *ci, JzREHeaderTool *tool) :
    m_visitor(&(ci->getASTContext()), tool) { }

void JzREHeaderToolASTConsumer::HandleTranslationUnit(clang::ASTContext &context)
{
    m_visitor.TraverseDecl(context.getTranslationUnitDecl());
}
