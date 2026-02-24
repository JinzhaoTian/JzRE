/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include <memory>
#include <vector>

#include <gtest/gtest.h>

#include "JzRE/CLI/JzCliCommandRegistry.h"

namespace {

class JzDummyCommand final : public JzRE::JzCliDomainCommand {
public:
    const JzRE::String &GetDomain() const override
    {
        static const JzRE::String kDomain = "dummy";
        return kDomain;
    }

    JzRE::JzCliResult Execute(JzRE::JzCliContext &,
                              const std::vector<JzRE::String> &,
                              JzRE::JzCliOutputFormat) override
    {
        return JzRE::JzCliResult::Ok("dummy-ok");
    }

    JzRE::String GetHelp() const override
    {
        return "  dummy    Dummy test command";
    }
};

} // namespace

TEST(JzCliCommandRegistry, RoutesToRegisteredDomain)
{
    JzRE::JzCliCommandRegistry registry;
    registry.Register(std::make_unique<JzDummyCommand>());

    JzRE::JzCliContext context;
    ASSERT_TRUE(context.Initialize());

    const auto result = registry.Execute("dummy", context, {}, JzRE::JzCliOutputFormat::Text);
    EXPECT_TRUE(result.IsSuccess());
    EXPECT_EQ(result.message, "dummy-ok");

    context.Shutdown();
}

TEST(JzCliCommandRegistry, RejectsUnknownDomain)
{
    JzRE::JzCliCommandRegistry registry;

    JzRE::JzCliContext context;
    ASSERT_TRUE(context.Initialize());

    const auto result = registry.Execute("unknown", context, {}, JzRE::JzCliOutputFormat::Text);
    EXPECT_FALSE(result.IsSuccess());
    EXPECT_EQ(result.code, JzRE::JzCliExitCode::InvalidArguments);

    context.Shutdown();
}

TEST(JzCliCommandRegistry, BuiltinDomainHelpIsRoutable)
{
    JzRE::JzCliCommandRegistry registry;
    registry.RegisterBuiltins();

    JzRE::JzCliContext context;
    ASSERT_TRUE(context.Initialize());

    const std::vector<JzRE::String> args = {"--help"};
    const auto result = registry.Execute("project", context, args, JzRE::JzCliOutputFormat::Text);
    EXPECT_TRUE(result.IsSuccess());
    EXPECT_NE(result.message.find("project commands"), JzRE::String::npos);

    context.Shutdown();
}
