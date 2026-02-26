/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#pragma once

#include "JzRE/CLI/JzCliCommandRegistry.h"

namespace JzRE {

/**
 * @brief `create` command — create project files (shaders, scripts, entities).
 */
class JzCreateCommand final : public JzCliDomainCommand {
public:
    [[nodiscard]] const String &GetDomain() const override;
    JzCliResult                 Execute(JzCliContext              &context,
                                        const std::vector<String> &args,
                                        JzCliOutputFormat          format) override;
    [[nodiscard]] String        GetHelp() const override;
};

} // namespace JzRE
