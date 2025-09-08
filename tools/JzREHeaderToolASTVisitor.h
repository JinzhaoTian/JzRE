#pragma once

#include "JzREHeaderToolTypes.h"
#include "JzREHeaderTool.h"

#include <unordered_map>
#include <string>

#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/DeclCXX.h"

/**
 * @brief JzRE Tool Clang AST Visitor
 */
class JzREHeaderToolASTVisitor : public clang::RecursiveASTVisitor<JzREHeaderToolASTVisitor> {
public:
    explicit JzREHeaderToolASTVisitor(clang::ASTContext *context, JzREHeaderTool *tool);

    bool VisitCXXRecordDecl(clang::CXXRecordDecl *declaration);

private:
    bool                                         HasReflectedClassMacro(const clang::CXXRecordDecl *classDecl);
    void                                         ProcessReflectedClass(const clang::CXXRecordDecl *classDecl);
    bool                                         HasReflectedPropertyMacro(const clang::FieldDecl *fieldDecl);
    JzREHeaderToolReflectedClassPropertyInfo     ProcessReflectedProperty(const clang::FieldDecl *fieldDecl);
    bool                                         HasReflectedMethodMacro(const clang::CXXMethodDecl *methodDecl);
    JzREHeaderToolReflectedClassMethodInfo       ProcessReflectedMethod(const clang::CXXMethodDecl *methodDecl);
    std::unordered_map<std::string, std::string> ParseReflectedMacroArgs(const std::string &macroArgs);
    std::string                                  ExtractMacroArgumentsFromSource(clang::SourceLocation startLoc, const std::string &macroName);

private:
    clang::ASTContext *m_context;
    JzREHeaderTool    *m_tool;
};
