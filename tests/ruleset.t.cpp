#include <codeowners/ruleset.hpp>

#include <gtest/gtest.h>

namespace co
{

TEST(parser, ruleset)
{
    rule_source src{"", 0};
    std::vector<annotated_rule> arules{
      {src, {pattern{"*.hpp"}, {owner{"octocat"}}}}};

    ruleset rset{arules};

    auto no_result = rset.apply("hello.cpp");
    EXPECT_FALSE(no_result);

    auto result = rset.apply("hello.hpp");
    ASSERT_TRUE(result);
    EXPECT_EQ(*result, arules.front());
};

} /* end namespace 'co' */
