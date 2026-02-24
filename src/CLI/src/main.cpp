/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include <algorithm>
#include <format>
#include <iostream>
#include <optional>
#include <vector>

#include <spdlog/logger.h>
#include <spdlog/spdlog.h>

#include "JzRE/CLI/JzCliCommandRegistry.h"
#include "JzRE/Runtime/Core/JzLogger.h"

namespace JzRE {

namespace {

constexpr const char *kCliVersion = "0.1.0";

std::optional<JzCliOutputFormat> ParseOutputFormat(const String &value)
{
    if (value == "text") return JzCliOutputFormat::Text;
    if (value == "json") return JzCliOutputFormat::Json;
    return std::nullopt;
}

Bool ApplyLogLevel(const String &value)
{
    auto &logger = JzLogger::GetInstance();
    (void)logger;

    auto activeLogger = spdlog::get("main_logger");
    if (!activeLogger) {
        return false;
    }

    if (value == "trace") {
        activeLogger->set_level(spdlog::level::trace);
        return true;
    }
    if (value == "debug") {
        activeLogger->set_level(spdlog::level::debug);
        return true;
    }
    if (value == "info") {
        activeLogger->set_level(spdlog::level::info);
        return true;
    }
    if (value == "warn") {
        activeLogger->set_level(spdlog::level::warn);
        return true;
    }
    if (value == "error") {
        activeLogger->set_level(spdlog::level::err);
        return true;
    }

    return false;
}

String BuildGlobalHelp(const JzCliCommandRegistry &registry)
{
    return std::format(
        "{}\nGlobal options:\n"
        "  --help, -h\n"
        "  --version\n"
        "  --format text|json\n"
        "  --log-level trace|debug|info|warn|error\n",
        registry.BuildHelpText());
}

}

} // namespace JzRE

int main(int argc, char **argv)
{
    using namespace JzRE;

    std::vector<String> rawTokens;
    rawTokens.reserve(static_cast<Size>(std::max(0, argc - 1)));
    for (int i = 1; i < argc; ++i) {
        rawTokens.emplace_back(argv[i]);
    }

    JzCliOutputFormat format      = JzCliOutputFormat::Text;
    Bool              showHelp    = false;
    Bool              showVersion = false;

    std::vector<String> commandTokens;
    commandTokens.reserve(rawTokens.size());

    for (Size i = 0; i < rawTokens.size(); ++i) {
        const auto &token = rawTokens[i];

        if (token == "--help" || token == "-h") {
            showHelp = true;
            continue;
        }

        if (token == "--version") {
            showVersion = true;
            continue;
        }

        if (token == "--format") {
            if (i + 1 >= rawTokens.size()) {
                std::cerr << "Missing value for --format" << std::endl;
                return static_cast<int>(JzCliExitCode::InvalidArguments);
            }

            auto parsed = ParseOutputFormat(rawTokens[++i]);
            if (!parsed.has_value()) {
                std::cerr << "Invalid --format value: " << rawTokens[i] << std::endl;
                return static_cast<int>(JzCliExitCode::InvalidArguments);
            }
            format = *parsed;
            continue;
        }

        if (token.rfind("--format=", 0) == 0) {
            auto parsed = ParseOutputFormat(token.substr(9));
            if (!parsed.has_value()) {
                std::cerr << "Invalid --format value: " << token.substr(9) << std::endl;
                return static_cast<int>(JzCliExitCode::InvalidArguments);
            }
            format = *parsed;
            continue;
        }

        if (token == "--log-level") {
            if (i + 1 >= rawTokens.size()) {
                std::cerr << "Missing value for --log-level" << std::endl;
                return static_cast<int>(JzCliExitCode::InvalidArguments);
            }

            if (!ApplyLogLevel(rawTokens[++i])) {
                std::cerr << "Invalid --log-level value: " << rawTokens[i] << std::endl;
                return static_cast<int>(JzCliExitCode::InvalidArguments);
            }
            continue;
        }

        if (token.rfind("--log-level=", 0) == 0) {
            auto value = token.substr(12);
            if (!ApplyLogLevel(value)) {
                std::cerr << "Invalid --log-level value: " << value << std::endl;
                return static_cast<int>(JzCliExitCode::InvalidArguments);
            }
            continue;
        }

        commandTokens.push_back(token);
    }

    JzCliCommandRegistry registry;
    registry.RegisterBuiltins();

    if (showVersion) {
        std::cout << "JzRE " << kCliVersion << std::endl;
        return static_cast<int>(JzCliExitCode::Success);
    }

    if (showHelp && commandTokens.empty()) {
        std::cout << BuildGlobalHelp(registry) << std::endl;
        return static_cast<int>(JzCliExitCode::Success);
    }

    if (commandTokens.empty()) {
        std::cerr << BuildGlobalHelp(registry) << std::endl;
        return static_cast<int>(JzCliExitCode::InvalidArguments);
    }

    const auto          domain = commandTokens.front();
    std::vector<String> domainArgs(commandTokens.begin() + 1, commandTokens.end());

    if (showHelp) {
        domainArgs.insert(domainArgs.begin(), "--help");
    }

    JzCliContext context;
    if (!context.Initialize()) {
        std::cerr << "Failed to initialize CLI context" << std::endl;
        return static_cast<int>(JzCliExitCode::RuntimeError);
    }

    const auto result = registry.Execute(domain, context, domainArgs, format);
    context.Shutdown();

    if (result.IsSuccess()) {
        if (!result.message.empty()) {
            std::cout << result.message << std::endl;
        }
    } else {
        if (!result.message.empty()) {
            std::cerr << result.message << std::endl;
        }
    }

    return static_cast<int>(result.code);
}
