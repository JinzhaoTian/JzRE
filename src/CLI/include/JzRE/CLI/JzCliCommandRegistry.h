/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "JzRE/CLI/JzCliContext.h"
#include "JzRE/CLI/JzCliTypes.h"

namespace JzRE {

/**
 * @brief Domain command interface.
 */
class JzCliDomainCommand {
public:
    virtual ~JzCliDomainCommand() = default;

    /**
     * @brief Get top-level domain token.
     *
     * @return const String& Domain token.
     */
    [[nodiscard]] virtual const String &GetDomain() const = 0;

    /**
     * @brief Execute a domain command.
     *
     * @param context CLI context.
     * @param args Domain-local arguments.
     * @param format Global output format.
     *
     * @return JzCliResult Execution result.
     */
    virtual JzCliResult Execute(JzCliContext &context,
                                const std::vector<String> &args,
                                JzCliOutputFormat          format) = 0;

    /**
     * @brief Get domain help text.
     *
     * @return String Help text.
     */
    [[nodiscard]] virtual String GetHelp() const = 0;
};

/**
 * @brief Registry for top-level CLI domain commands.
 */
class JzCliCommandRegistry {
public:
    /**
     * @brief Register a domain command implementation.
     *
     * @param command Command object.
     */
    void Register(std::unique_ptr<JzCliDomainCommand> command);

    /**
     * @brief Register built-in domain commands.
     */
    void RegisterBuiltins();

    /**
     * @brief Execute command by domain token.
     *
     * @param domain Domain token.
     * @param context CLI context.
     * @param args Domain-local args.
     * @param format Output format.
     *
     * @return JzCliResult Execution result.
     */
    JzCliResult Execute(const String              &domain,
                        JzCliContext              &context,
                        const std::vector<String> &args,
                        JzCliOutputFormat          format) const;

    /**
     * @brief Build aggregate help text for all domains.
     *
     * @return String Help text.
     */
    [[nodiscard]] String BuildHelpText() const;

private:
    std::unordered_map<String, std::unique_ptr<JzCliDomainCommand>> m_commands;
};

} // namespace JzRE
