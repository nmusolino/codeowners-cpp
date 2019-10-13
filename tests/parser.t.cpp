#include <codeowners/parser.hpp>

#include <gtest/gtest.h>

namespace co
{
namespace testing
{

TEST(parser, line_range)
{
    std::istringstream is{"a\nbb\n\nccc"};
    auto line_rng = line_range(is);

    std::vector<std::string> lines{line_rng.begin(), line_rng.end()};
    EXPECT_EQ(lines, (std::vector<std::string>{{"a", "bb", "", "ccc"}}));
};

TEST(parser, parse_non_rule)
{
    for (const auto line : {"", "\t", "  ", "# comment", "  # comment"})
    {
        EXPECT_EQ(parse_line(line), std::nullopt);
    }
};

TEST(parser, parse_valid_rules)
{
    // Examples from https://help.github.com/en/articles/about-code-owners
    EXPECT_EQ(parse_line("docs/*  docs@example.com"),
              (ownership_rule{pattern{"docs/*"}, {owner{"docs@example.com"}}}));
    EXPECT_EQ(parse_line("/docs/  @doctocat"),
              (ownership_rule{pattern{"/docs/"}, {owner{"@doctocat"}}}));
    EXPECT_EQ(parse_line("apps/  @octocat"),
              (ownership_rule{pattern{"apps/"}, {owner{"@octocat"}}}));
};

TEST(parser, parse)
{
    const char* content = R"(# Comment
        docs/*  docs@example.com

        # Comment
        apps/  @octocat
    )";

    std::istringstream is{content};
    auto ann_rules = parse(is, ".github/codeowners");

    annotated_rule expected_rule1 = {
      rule_source{".github/codeowners", 2},
      ownership_rule{pattern{"docs/*"}, {owner{"docs@example.com"}}}};

    annotated_rule expected_rule2 = {
      rule_source{".github/codeowners", 5},
      ownership_rule{pattern{"apps/"}, {owner{"@octocat"}}}};

    EXPECT_EQ(ann_rules,
              (std::vector<annotated_rule>{{expected_rule1, expected_rule2}}));
}

} /* end namespace 'testing' */
} /* end namespace 'co' */
