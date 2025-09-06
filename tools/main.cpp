#include "JzREHeaderToolOptions.h"
#include "JzREHeaderToolFrontend.h"
#include "JzREHeaderTool.h"

#include "clang/Tooling/CommonOptionsParser.h"

int main(int argc, const char **argv)
{
    auto expectedParser = clang::tooling::CommonOptionsParser::create(argc, argv, JzREHeaderToolOptionCategory);
    if (!expectedParser) {
        return 1;
    }

    auto                     &optionParser = expectedParser.get();
    clang::tooling::ClangTool Tool(optionParser.getCompilations(), optionParser.getSourcePathList());

    // 创建工具实例
    JzREHeaderTool headerTool;

    // 预处理阶段
    headerTool.PreprocessPhase(optionParser.getSourcePathList());

    // 运行Clang工具进行解析
    JzREHeaderToolFrontendActionFactory factory(&headerTool);
    int                                 result = Tool.run(&factory);

    if (result == 0) {
        // 验证和生成阶段
        headerTool.ValidatePhase();
        headerTool.GeneratePhase();
    }

    return result;
}
