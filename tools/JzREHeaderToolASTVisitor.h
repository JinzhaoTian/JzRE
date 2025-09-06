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
    bool                                         HasReflectedClassAttribute(const clang::CXXRecordDecl *classDecl);
    void                                         ProcessReflectedClass(const clang::CXXRecordDecl *classDecl);
    bool                                         HasReflectedPropertyAttribute(const clang::FieldDecl *fieldDecl);
    JzREHeaderToolReflectedClassPropertyInfo     ProcessReflectedProperty(const clang::FieldDecl *fieldDecl);
    bool                                         HasReflectedMethodAttribute(const clang::CXXMethodDecl *methodDecl);
    JzREHeaderToolReflectedClassMethodInfo       ProcessReflectedMethod(const clang::CXXMethodDecl *methodDecl);
    std::unordered_map<std::string, std::string> ParseReflectedAnnotation(const std::string &annotation);

private:
    clang::ASTContext *m_context;
    JzREHeaderTool    *m_tool;
};
