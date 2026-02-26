/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#pragma once

#include "JzRE/CLI/JzCliCommandRegistry.h"

namespace JzRE {

/**
 * @brief `import` command — import assets into a project.
 */
class JzImportCommand final : public JzCliDomainCommand {
public:
    [[nodiscard]] const String &GetDomain() const override;
    JzCliResult                 Execute(JzCliContext              &context,
                                        const std::vector<String> &args,
                                        JzCliOutputFormat          format) override;
    [[nodiscard]] String        GetHelp() const override;
};

} // namespace JzRE
