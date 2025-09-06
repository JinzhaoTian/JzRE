#pragma once

#include "llvm/Support/CommandLine.h"

/**
 * @brief JzRE Tool Clang Command Line Options
 */
extern llvm::cl::OptionCategory   JzREHeaderToolOptionCategory;
extern llvm::cl::extrahelp        JzREHeaderToolHelp;
extern llvm::cl::opt<std::string> JzREHeaderToolArgSource;
extern llvm::cl::opt<std::string> JzREHeaderToolArgOutput;
extern llvm::cl::opt<bool>        JzREHeaderToolArgVerbose;
