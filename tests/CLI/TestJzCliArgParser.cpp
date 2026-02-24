/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include <unordered_set>
#include <vector>

#include <gtest/gtest.h>

#include "JzRE/CLI/JzCliArgParser.h"

TEST(JzCliArgParser, ParsesFlagsAndOptionValues)
{
    const std::vector<JzRE::String> tokens = {
        "--project", "Test.jzreproject",
        "--src", "a.png", "b.png",
        "--overwrite",
        "import"
    };

    const std::unordered_set<JzRE::String> flags = {"--overwrite"};
    const auto parsed = JzRE::JzCliArgParser::Parse(tokens, flags);

    ASSERT_TRUE(parsed.HasOption("--project"));
    ASSERT_TRUE(parsed.HasOption("--src"));
    ASSERT_TRUE(parsed.HasOption("--overwrite"));

    const auto *project = parsed.GetFirstValue("--project");
    ASSERT_NE(project, nullptr);
    EXPECT_EQ(*project, "Test.jzreproject");

    const auto *src = parsed.GetOptionValues("--src");
    ASSERT_NE(src, nullptr);
    ASSERT_EQ(src->size(), 2U);
    EXPECT_EQ(src->at(0), "a.png");
    EXPECT_EQ(src->at(1), "b.png");

    ASSERT_EQ(parsed.positionals.size(), 1U);
    EXPECT_EQ(parsed.positionals.front(), "import");
}

TEST(JzCliArgParser, ParsesEqualsForm)
{
    const std::vector<JzRE::String> tokens = {
        "--format=json",
        "--project=MyProject.jzreproject"
    };

    const auto parsed = JzRE::JzCliArgParser::Parse(tokens);

    const auto *format = parsed.GetFirstValue("--format");
    ASSERT_NE(format, nullptr);
    EXPECT_EQ(*format, "json");

    const auto *project = parsed.GetFirstValue("--project");
    ASSERT_NE(project, nullptr);
    EXPECT_EQ(*project, "MyProject.jzreproject");
}

TEST(JzCliArgParser, SupportsShortOptionToken)
{
    const std::vector<JzRE::String> tokens = {
        "-h",
        "project"
    };

    const auto parsed = JzRE::JzCliArgParser::Parse(tokens);
    ASSERT_TRUE(parsed.HasOption("-h"));
    ASSERT_EQ(parsed.positionals.size(), 1U);
    EXPECT_EQ(parsed.positionals.front(), "project");
}

TEST(JzCliArgParser, HandlesMissingValueAsEmptyOption)
{
    const std::vector<JzRE::String> tokens = {
        "--project",
        "--src",
        "a.png"
    };

    const auto parsed = JzRE::JzCliArgParser::Parse(tokens);
    ASSERT_TRUE(parsed.HasOption("--project"));
    EXPECT_EQ(parsed.GetFirstValue("--project"), nullptr);

    const auto *src = parsed.GetOptionValues("--src");
    ASSERT_NE(src, nullptr);
    ASSERT_EQ(src->size(), 1U);
    EXPECT_EQ(src->front(), "a.png");
}

TEST(JzCliArgParser, KeepsRepeatedOptionValues)
{
    const std::vector<JzRE::String> tokens = {
        "--format", "text",
        "--format", "json"
    };

    const auto parsed = JzRE::JzCliArgParser::Parse(tokens);
    const auto *values = parsed.GetOptionValues("--format");
    ASSERT_NE(values, nullptr);
    ASSERT_EQ(values->size(), 2U);
    EXPECT_EQ(values->at(0), "text");
    EXPECT_EQ(values->at(1), "json");
}
