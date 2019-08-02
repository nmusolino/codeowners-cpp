#include <codeowners/filesystem.hpp>
#include <codeowners/git.hpp>

#include <gtest/gtest.h>

namespace co
{
namespace testing
{

TEST(create_repository_test, creates_repository)
{
    temporary_directory_handle temp_dir;
    repository_ptr repo_ptr = create_repository(temp_dir);
    ASSERT_TRUE(repo_ptr);
};

TEST(attribute_set, construction)
{
    const std::string attribute_name { "my_attribute" };
    attribute_set attr_set { attribute_name };
    EXPECT_EQ(attr_set.attribute_name(), attribute_name);
};

TEST(pattern_value_set, pattern)
{
    const std::string attribute_name { "my_attribute" };
    attribute_set attr_set { attribute_name };
    attr_set.add_pattern(pattern { "*.hpp" }, "x");
    attr_set.add_pattern(pattern { "*.cpp" }, "y");

    EXPECT_EQ(attr_set.get("sample.hpp"), "x");
    EXPECT_EQ(attr_set.get("sample.cpp"), "y");
};

TEST(pattern_value_set, undefined_value)
{
    const std::string attribute_name { "my_attribute" };
    attribute_set attr_set { attribute_name };
    attr_set.add_pattern(pattern { "*.hpp" }, "foo");

    EXPECT_FALSE(bool(attr_set.get_optional("unmatched_file")));
    EXPECT_THROW(attr_set.get("unmatched_file"), attribute_set::no_attribute_error);
};

}  // end namespace 'testing'
}  // end namespace 'co'
