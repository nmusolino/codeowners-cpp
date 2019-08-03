#include <codeowners/git_resources.hpp>

#include <gtest/gtest.h>


namespace co
{
namespace testing
{

TEST(attribute_set_test, pattern)
{
    const std::string attribute_name{"my_attribute"};
    attribute_set attr_set{attribute_name};
    attr_set.add_pattern(pattern{"*.hpp"}, "x");
    attr_set.add_pattern(pattern{"*.cpp"}, "y");

    EXPECT_EQ(attr_set.get("sample.hpp"), "x");
    EXPECT_EQ(attr_set.get("sample.cpp"), "y");
};

TEST(attribute_set_test, undefined_value)
{
    const std::string attribute_name{"my_attribute"};
    attribute_set attr_set{attribute_name};
    attr_set.add_pattern(pattern{"*.hpp"}, "foo");

    EXPECT_FALSE(bool(attr_set.get_optional("unmatched_file")));
    EXPECT_THROW(attr_set.get("unmatched_file"),
                 attribute_set::no_attribute_error);
};

} // end namespace 'testing'
} // end namespace 'co'
