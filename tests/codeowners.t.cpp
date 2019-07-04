#include "codeowners/codeowners.hpp"
#include "codeowners/paths.hpp"

#include <gtest/gtest.h>

#include <boost/process.hpp>

#include <cstdio>

namespace co {
namespace testing {

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

        for (char c : { 'a', 'b', 'c' }) {
            using namespace std::string_literals;
            fs::path d = repository_root / ("dir_"s + c);
            fs::create_directory(d);
            for (int i : { 0, 1, 2 }) {
                touch(d / ("file_"s + std::to_string(i)));
            }
            git(repository_root, "add", d.string());
        }
        git(repository_root, "commit", "-m", "Initial commit");
        return repository_root;
    }

    TEST(PathsTest, Touch)
    {
        fs::path p = temp_directory() / "sample_file";
        EXPECT_FALSE(fs::exists(p));
        co::touch(p);
        EXPECT_TRUE(fs::exists(p)) << "p: " << p;
        EXPECT_TRUE(fs::is_regular_file(p));
    }

    TEST(CodeownersTest, TestRepositoryConstructibleFromChildPaths)
    {
        fs::path repo_root = temp_directory() / "sample_repository";
        populate_sample_repository(repo_root);

        // Test that these do not throw.
        repository { repo_root };
        repository { repo_root / "dir_a" };
        repository { repo_root / "dir_a" / "file_0" };
    }

    TEST(CodeownersTest, TestRepositoryContains)
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

} /* end namespace 'testing' */
} /* end namespace 'co' */
