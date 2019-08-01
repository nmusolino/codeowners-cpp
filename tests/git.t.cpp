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

TEST(pattern_value_set, construction)
{
    const std::string attribute_name { "my_attribute" };
    pattern_value_set pvs { attribute_name };
    EXPECT_EQ(pvs.attribute_name(), attribute_name);
};

TEST(pattern_value_set, pattern)
{
    const std::string attribute_name { "my_attribute" };
    pattern_value_set pvs { attribute_name };

    pvs.add_pattern("sample.hpp", 42);
    EXPECT_EQ(pvs.get<int>("sample.hpp"), 42);

    EXPECT_EQ(pvs.attribute_name(), attribute_name);
};

TEST(pattern_value_set, undefined_value)
{
    const std::string attribute_name { "my_attribute" };
    pattern_value_set pvs { attribute_name };

    pvs.add_pattern("*.hpp", 42);
    EXPECT_FALSE(bool(pvs.get_optional<int>("unmatched_file")));
    EXPECT_THROW(pvs.get<int>("unmatched_file"), pattern_value_set::no_attribute_value);
};

}  // end namespace 'testing'
}  // end namespace 'co'
