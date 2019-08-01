#include "codeowners/codeowners.hpp"
#include "codeowners/filesystem.hpp"

#include <gtest/gtest.h>

#include <boost/process.hpp>

#include <cstdio>

namespace co
{
namespace testing
{

fs::path temp_directory()
{
    fs::path directory = fs::temp_directory_path() / fs::unique_path() / "codeowners_test";
    fs::create_directories(directory);
    return directory;
}

template <typename... Args>
void git(const fs::path& repository_root, Args&&... args)
{
    boost::process::system(boost::process::search_path("git"),
        std::forward<Args>(args)...,
        boost::process::start_dir = repository_root.string(),
        boost::process::std_out > boost::process::null,
        boost::process::throw_on_error);
}

fs::path populate_sample_repository(const fs::path& repository_root)
{
    fs::create_directories(repository_root);

    assert(fs::exists(repository_root));
    git(repository_root, "init");

    for (char c : { 'a', 'b', 'c' })
    {
        using namespace std::string_literals;
        fs::path d = repository_root / ("dir_"s + c);
        fs::create_directory(d);
        for (int i : { 0, 1, 2 })
        {
            touch(d / ("file_"s + std::to_string(i)));
        }
        git(repository_root, "add", d.string());
    }
    git(repository_root, "commit", "-m", "Initial commit");
    return repository_root;
}

TEST(paths_test, touch)
{
    fs::path p = temp_directory() / "sample_file";
    EXPECT_FALSE(fs::exists(p));
    co::touch(p);
    EXPECT_TRUE(fs::exists(p)) << "p: " << p;
    EXPECT_TRUE(fs::is_regular_file(p));
}

TEST(codeowners_test, test_codeowners_file)
{
    fs::path repo_root = temp_directory() / "sample_repository";
    fs::create_directories(repo_root);

    EXPECT_FALSE(codeowners_file(repo_root).has_value());

    fs::path actual_path = repo_root / "CODEOWNERS";
    co::touch(actual_path);

    std::optional<fs::path> result = codeowners_file(repo_root);
    ASSERT_TRUE(result.has_value())
        << "actual_path: " << actual_path << " (exists: " << std::boolalpha << fs::exists(actual_path) << ")";
    EXPECT_EQ(*result, actual_path);
};

TEST(codeowners_test, repository_constructible_from_child)
{
    fs::path repo_root = temp_directory() / "sample_repository";
    populate_sample_repository(repo_root);

    // Test that these do not throw.
    repository { repo_root };
    repository { repo_root / "dir_a" };
    repository { repo_root / "dir_a" / "file_0" };
};

TEST(codeowners_test, repository_contains)
{
    fs::path repo_root = temp_directory() / "sample_repository";
    populate_sample_repository(repo_root);

    repository repo(repo_root);

    ASSERT_TRUE(repo.contains(repo_root / "dir_a" / "file_0"));

    // Whether this file exists or not, `contains()` should return false.
    fs::path sample_file = repo_root / "sample_file";
    ASSERT_FALSE(repo.contains(sample_file));
    touch(sample_file);
    ASSERT_FALSE(repo.contains(sample_file));
}

TEST(codeowners_test, pattern_match_bare_star)
{
    file_pattern pat { "*" };
    EXPECT_TRUE(pat.match("foo"));
    EXPECT_TRUE(pat.match("foo/"));
    EXPECT_TRUE(pat.match("foo/bar"));
    EXPECT_TRUE(pat.match("foo/bar/baz"));
}

TEST(codeowners_test, pattern_match_star_extension)
{
    file_pattern pat { "*.cpp" };
    EXPECT_TRUE(pat.match("file.cpp"));
    EXPECT_TRUE(pat.match("include/file.cpp"));
    EXPECT_TRUE(pat.match("include/utils/file.cpp"));

    EXPECT_FALSE(pat.match("include"));
    EXPECT_FALSE(pat.match("file.hpp"));
}

TEST(codeowners_test, pattern_match_leading_slash)
{
    file_pattern pat { "/build/logs/" };
    EXPECT_TRUE(pat.match("build/logs"));
    EXPECT_TRUE(pat.match("build/logs/log1.txt"));

    EXPECT_FALSE(pat.match("parent/build/logs"));
    EXPECT_FALSE(pat.match("parent/build/logs"));
}

TEST(codeowners_test, pattern_match_trailing_star)
{
    file_pattern pat { "docs/*" };
    EXPECT_TRUE(pat.match("docs/getting-started.md"));
}

TEST(codeowners_test, pattern_match_anywhere_dir)
{
    file_pattern pat { "apps/" };
    EXPECT_TRUE(pat.match("apps"));
}

TEST(codeowners_test, DISABLED_nonimplemented_patterns)
{
    // These are known failures.
    EXPECT_TRUE(file_pattern { "apps/" }.match("parent/apps"));

    EXPECT_FALSE(file_pattern { "docs/*" }.match("docs/build-app/troubleshooting.md"));
}

} /* end namespace 'testing' */
} /* end namespace 'co' */
