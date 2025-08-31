#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"
#include <fstream>

using namespace clang;
using namespace clang::tooling;

// 定义应用选项
static llvm::cl::OptionCategory   MyToolCategory("Reflection Generator Options");
static llvm::cl::extrahelp        CommonHelp(CommonOptionsParser::HelpMessage);
static llvm::cl::opt<std::string> OutputFilename("o", llvm::cl::desc("Specify output filename"), llvm::cl::value_desc("filename"), llvm::cl::cat(MyToolCategory));

// AST Visitor 用于查找标记了反射宏的类
class ReflectionVisitor : public RecursiveASTVisitor<ReflectionVisitor> {
public:
    explicit ReflectionVisitor(ASTContext *Context, std::ostream &OS) :
        Context(Context), OutStream(OS) { }

    // 访问CXXRecordDecl（类/结构体/联合体）
    bool VisitCXXRecordDecl(CXXRecordDecl *Declaration)
    {
        // 检查该类是否有我们定义的反射属性或注解
        if (hasReflectionAttribute(Declaration)) {
            // 处理这个需要反射的类
            processReflectedClass(Declaration);
        }
        return true;
    }

private:
    ASTContext   *Context;
    std::ostream &OutStream;

    bool hasReflectionAttribute(const CXXRecordDecl *Decl)
    {
        // 检查 Decl 是否包含特定的反射属性或注解
        // 例如：__attribute__((reflectable)) 或 [[reflectable]]
        // 这里简化处理：检查是否有特定注解（Clang 的 AnnotateAttr）
        for (const auto *Attr : Decl->attrs()) {
            if (const auto *Annotate = dyn_cast<AnnotateAttr>(Attr)) {
                if (Annotate->getAnnotation() == "reflectable") {
                    return true;
                }
            }
        }
        // 或者，也可以检查特定的宏（在AST中宏已展开，较难直接检测）
        // 更常见的做法是使用自定义注解（attribute）
        return false;
    }

    void processReflectedClass(const CXXRecordDecl *ClassDecl)
    {
        std::string ClassName = ClassDecl->getQualifiedNameAsString();
        OutStream << "// Reflected class: " << ClassName << "\n";

        // 生成类型信息结构体
        OutStream << "template <>\n";
        OutStream << "struct TypeInfo<" << ClassName << "> {\n";
        OutStream << "  static constexpr const char* name() { return \"" << ClassName << "\"; }\n";
        OutStream << "  static constexpr size_t size() { return sizeof(" << ClassName << "); }\n";
        // ... 可以生成更多信息

        // 遍历成员变量
        OutStream << "  struct Members {\n";
        for (const auto *Field : ClassDecl->fields()) {
            if (Field->getAccess() != AS_private) { // 通常只反射非私有成员
                std::string FieldName = Field->getNameAsString();
                QualType    FieldType = Field->getType();
                std::string TypeName  = FieldType.getAsString();
                // 需要对 TypeName 进行一些清理和标准化（去除限定符等）
                OutStream << "    // Field: " << TypeName << " " << FieldName << "\n";
                // 生成字段的元信息（需要更复杂的逻辑来处理偏移量、类型映射等）
            }
        }
        OutStream << "  };\n";
        OutStream << "};\n\n";

        // 生成全局注册函数
        OutStream << "namespace {\n";
        OutStream << "  static bool " << ClassName << "_registered = []() {\n";
        OutStream << "    ReflectionRegistry::registerType<TypeInfo<" << ClassName << ">>();\n";
        OutStream << "    return true;\n";
        OutStream << "  }();\n";
        OutStream << "}\n\n";
    }
};

// AST Consumer
class ReflectionASTConsumer : public ASTConsumer {
public:
    explicit ReflectionASTConsumer(CompilerInstance *CI, std::ostream &OS) :
        Visitor(&(CI->getASTContext()), OS) { }

    void HandleTranslationUnit(ASTContext &Context) override
    {
        Visitor.TraverseDecl(Context.getTranslationUnitDecl());
    }

private:
    ReflectionVisitor Visitor;
};

// Frontend Action
class ReflectionFrontendAction : public ASTFrontendAction {
public:
    ReflectionFrontendAction(std::ostream &OS) :
        OutStream(OS) { }

    std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI, StringRef file) override
    {
        return std::make_unique<ReflectionASTConsumer>(&CI, OutStream);
    }

private:
    std::ostream &OutStream;
};

// Frontend Action Factory
class ReflectionFrontendActionFactory : public FrontendActionFactory {
public:
    ReflectionFrontendActionFactory(std::ostream &OS) :
        OutStream(OS) { }

    std::unique_ptr<FrontendAction> create() override
    {
        return std::make_unique<ReflectionFrontendAction>(OutStream);
    }

private:
    std::ostream &OutStream;
};

int main(int argc, const char **argv)
{
    // 解析命令行参数
    CommonOptionsParser OptionsParser(argc, argv, MyToolCategory);
    ClangTool           Tool(OptionsParser.getCompilations(), OptionsParser.getSourcePathList());

    // 打开输出文件
    std::error_code      EC;
    llvm::raw_fd_ostream OutFile(OutputFilename.c_str(), EC);
    if (EC) {
        llvm::errs() << "Error opening output file: " << EC.message() << "\n";
        return 1;
    }

    // 运行工具
    ReflectionFrontendActionFactory Factory(OutFile);
    return Tool.run(&Factory);
}