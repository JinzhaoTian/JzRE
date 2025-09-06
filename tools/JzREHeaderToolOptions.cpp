#include "JzREHeaderToolOptions.h"
#include "clang/Tooling/CommonOptionsParser.h"

/**
 * @brief JzRE Tool Clang Command Line Option
 */
llvm::cl::OptionCategory   JzREHeaderToolOptionCategory("JzRE Header Tool options");
llvm::cl::extrahelp        JzREHeaderToolHelp(clang::tooling::CommonOptionsParser::HelpMessage);
llvm::cl::opt<std::string> JzREHeaderToolArgSource("source",
                                                   llvm::cl::desc("Source directory"),
                                                   llvm::cl::value_desc("directory"),
                                                   llvm::cl::cat(JzREHeaderToolOptionCategory));
llvm::cl::opt<std::string> JzREHeaderToolArgOutput("output",
                                                   llvm::cl::desc("Output directory for generated files"),
                                                   llvm::cl::value_desc("directory"),
                                                   llvm::cl::cat(JzREHeaderToolOptionCategory));
llvm::cl::opt<bool>        JzREHeaderToolArgVerbose("verbose",
                                                    llvm::cl::desc("Enable verbose output"),
                                                    llvm::cl::cat(JzREHeaderToolOptionCategory));
