/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#include "JzRE/CLI/JzCliCommandRegistry.h"

#include <format>
#include <sstream>

#include "JzRE/CLI/commands/JzBuildCommand.h"
#include "JzRE/CLI/commands/JzCreateCommand.h"
#include "JzRE/CLI/commands/JzImportCommand.h"
#include "JzRE/CLI/commands/JzInitCommand.h"
#include "JzRE/CLI/commands/JzRunCommand.h"

namespace JzRE {

void JzCliCommandRegistry::Register(std::unique_ptr<JzCliDomainCommand> command)
{
    if (!command) {
        return;
    }

    m_commands[command->GetDomain()] = std::move(command);
}

void JzCliCommandRegistry::RegisterBuiltins()
{
    Register(std::make_unique<JzInitCommand>());
    Register(std::make_unique<JzCreateCommand>());
    Register(std::make_unique<JzImportCommand>());
    Register(std::make_unique<JzBuildCommand>());
    Register(std::make_unique<JzRunCommand>());
}

JzCliResult JzCliCommandRegistry::Execute(const String              &domain,
                                          JzCliContext              &context,
                                          const std::vector<String> &args,
                                          JzCliOutputFormat          format) const
{
    auto iter = m_commands.find(domain);
    if (iter == m_commands.end()) {
        return JzCliResult::Error(
            JzCliExitCode::InvalidArguments,
            std::format("Unknown command '{}'.\n\n{}", domain, BuildHelpText()));
    }

    return iter->second->Execute(context, args, format);
}

String JzCliCommandRegistry::BuildHelpText() const
{
    std::ostringstream ss;
    ss << "Usage: JzRE <command> [options]\n\n";
    ss << "Commands:\n";

    // Emit in logical workflow order
    const std::vector<String> order = {"init", "create", "import", "build", "run"};
    for (const auto &name : order) {
        auto iter = m_commands.find(name);
        if (iter != m_commands.end()) {
            ss << iter->second->GetHelp() << "\n";
        }
    }

    return ss.str();
}

} // namespace JzRE
