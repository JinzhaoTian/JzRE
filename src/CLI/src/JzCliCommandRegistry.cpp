/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/CLI/JzCliCommandRegistry.h"

#include <format>
#include <sstream>

#include "JzRE/CLI/commands/JzAssetCommand.h"
#include "JzRE/CLI/commands/JzProjectCommand.h"
#include "JzRE/CLI/commands/JzRunCommand.h"
#include "JzRE/CLI/commands/JzSceneCommand.h"
#include "JzRE/CLI/commands/JzShaderCommand.h"

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
    Register(std::make_unique<JzProjectCommand>());
    Register(std::make_unique<JzAssetCommand>());
    Register(std::make_unique<JzShaderCommand>());
    Register(std::make_unique<JzSceneCommand>());
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
            std::format("Unknown domain '{}'.\n\n{}", domain, BuildHelpText()));
    }

    return iter->second->Execute(context, args, format);
}

String JzCliCommandRegistry::BuildHelpText() const
{
    std::ostringstream ss;
    ss << "Usage: JzRE <domain> <command> [options]\n\n";
    ss << "Domains:\n";

    for (const auto &[name, command] : m_commands) {
        ss << command->GetHelp() << "\n";
    }

    return ss.str();
}

} // namespace JzRE
