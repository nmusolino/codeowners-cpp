#include <codeowners/filesystem.hpp>
#include <codeowners/git.hpp>

#include <gtest/gtest.h>

#include <boost/process.hpp>

namespace co
{
namespace testing
{

template <typename... Args>
void
git(const fs::path& repository_root, Args&&... args)
{
    boost::process::system(boost::process::search_path("git"),
                           std::forward<Args>(args)...,
                           boost::process::start_dir = repository_root.string(),
                           boost::process::std_out > boost::process::null,
                           boost::process::throw_on_error);
}

/// Return whether the two paths are equivalent, i.e. refer to the same
/// filesystem entity.
::testing::AssertionResult
equivalent(const fs::path& p1, const fs::path& p2)
{
    if (fs::equivalent(p1, p2))
    {
        return ::testing::AssertionSuccess()
               << "Path " << p1 << " is equivalent to " << p2;
    }
    else
    {
        return ::testing::AssertionFailure()
               << "Path " << p1 << " is not equivalent to " << p2;
    }
}

TEST(discover_repository_test, no_repository)
{
    temporary_directory_handle temp_dir;
    const bool across_fs = false;

    std::optional<fs::path> result = discover_repository(temp_dir, across_fs);
    EXPECT_FALSE(result);

    result = discover_repository(temp_dir / "nonexistent_dir", across_fs);
    EXPECT_FALSE(result);
};

TEST(discover_repository_test, repository)
{
    temporary_directory_handle temp_dir;
    const bool across_fs = false;

    git(temp_dir, "init");
    auto git_dir = temp_dir / ".git";
    ASSERT_TRUE(fs::exists(git_dir))
      << "Expected .git directory to exist after init operation";

    // Confirm that beginning with the .git directory locates the git directory.
    // NOTE: Google Test bug #1614 applies when printing two fs::path objects.
    // https://github.com/google/googletest/issues/1614
    std::optional<fs::path> result = discover_repository(git_dir, across_fs);
    ASSERT_TRUE(result);
    EXPECT_TRUE(co::testing::equivalent(*result, git_dir));

    // Confirm that beginning in a subdirectory of the working directory
    // locates the git directory.
    fs::create_directory(temp_dir / "a");
    result = discover_repository(temp_dir / "a", across_fs);
    ASSERT_TRUE(result);
    EXPECT_TRUE(co::testing::equivalent(*result, git_dir));
};

TEST(create_repository_test, creates_repository)
{
    temporary_directory_handle temp_dir;
    repository_ptr repo_ptr = create_repository(temp_dir);
    ASSERT_TRUE(repo_ptr);
};

TEST(attribute_set, construction)
{
    const std::string attribute_name{"my_attribute"};
    attribute_set attr_set{attribute_name};
    EXPECT_EQ(attr_set.attribute_name(), attribute_name);
};

TEST(pattern_value_set, pattern)
{
    const std::string attribute_name{"my_attribute"};
    attribute_set attr_set{attribute_name};
    attr_set.add_pattern(pattern{"*.hpp"}, "x");
    attr_set.add_pattern(pattern{"*.cpp"}, "y");

    EXPECT_EQ(attr_set.get("sample.hpp"), "x");
    EXPECT_EQ(attr_set.get("sample.cpp"), "y");
};

TEST(pattern_value_set, undefined_value)
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
