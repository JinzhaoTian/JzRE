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

    // 文件过滤
    std::vector<std::string> filteredFiles;
    for (const auto &file : optionParser.getSourcePathList()) {
        // 检查文件名是否为 JzReflectable.h 或包含该文件名
        if (file.find("JzReflectable.h") == std::string::npos) {
            filteredFiles.push_back(file);
        }
    }

    headerTool.PreprocessPhase(filteredFiles);

    JzREHeaderToolFrontendActionFactory factory(&headerTool);
    int                                 result = Tool.run(&factory);

    if (result == 0) {
        headerTool.ValidatePhase();
        headerTool.GeneratePhasePerFile();
    }

    return result;
}
