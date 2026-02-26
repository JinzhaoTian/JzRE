/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#pragma once

#include <filesystem>

#include "JzRE/CLI/JzCliCommandRegistry.h"

namespace JzRE {

/**
 * @brief `build` command — cook project content (shaders, etc.).
 */
class JzBuildCommand final : public JzCliDomainCommand {
public:
    [[nodiscard]] const String &GetDomain() const override;
    JzCliResult                 Execute(JzCliContext              &context,
                                        const std::vector<String> &args,
                                        JzCliOutputFormat          format) override;
    [[nodiscard]] String        GetHelp() const override;

    /**
     * @brief Run build for a resolved project file path.
     *        Used by JzRunCommand to trigger an auto-build.
     *
     * @param context CLI context.
     * @param projectPath Absolute path to .jzreproject file.
     * @param format Output format.
     * @param toolOverride Optional path to override the shader tool binary.
     *
     * @return JzCliResult Build result.
     */
    static JzCliResult BuildProject(JzCliContext                &context,
                                    const std::filesystem::path &projectPath,
                                    JzCliOutputFormat            format,
                                    const String               *toolOverride = nullptr);
};

} // namespace JzRE
