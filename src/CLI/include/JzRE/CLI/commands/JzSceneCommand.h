/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/CLI/JzCliCommandRegistry.h"

namespace JzRE {

/**
 * @brief Scene file utility CLI command domain.
 */
class JzSceneCommand final : public JzCliDomainCommand {
public:
    [[nodiscard]] const String &GetDomain() const override;
    JzCliResult Execute(JzCliContext &context,
                        const std::vector<String> &args,
                        JzCliOutputFormat          format) override;
    [[nodiscard]] String GetHelp() const override;
};

} // namespace JzRE
