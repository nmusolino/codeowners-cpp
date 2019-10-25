#include <codeowners/repository.hpp>

#include "tests/test_utils.hpp"

#include <codeowners/errors.hpp>
#include <codeowners/filesystem.hpp>

#include <gtest/gtest.h>

#include <boost/process.hpp>

namespace co
{

TEST(discover_repository_test, no_repository)
{
    temporary_directory_handle temp_dir;

    // When a directory does not contain a git repository, attempting to
    // discover a repository fails.
    EXPECT_THROW(repository::discover(temp_dir), co::repository_not_found_error);
    std::optional<repository> result = repository::try_discover(temp_dir);
    EXPECT_FALSE(result);

    // When a directory does not exist, both `discover` and `try_discover`
    // will raise an exception.
    EXPECT_THROW(repository::discover(temp_dir / "nonexistent_dir"), co::file_not_found_error);
    EXPECT_THROW(repository::try_discover(temp_dir / "nonexistent_dir"), co::file_not_found_error);
};

TEST(create_repository_test, creates_repository)
{
    temporary_directory_handle temp_dir;
    repository repo = repository::create(temp_dir);
    EXPECT_TRUE(repo.is_empty());
    EXPECT_FALSE(repo.is_bare());
    EXPECT_PATHS_EQUIVALENT(repo.work_directory(), temp_dir);
    EXPECT_PATHS_EQUIVALENT(repo.git_directory(), temp_dir / ".git");
    EXPECT_PATHS_EQUIVALENT(repo.common_directory(), temp_dir / ".git");
};

TEST(create_repository_test, creates_repository_bare)
{
    temporary_directory_handle temp_dir;
    repository repo = repository::create(temp_dir, repo_structure::BARE);
    EXPECT_TRUE(repo.is_empty());
    EXPECT_TRUE(repo.is_bare());
};

TEST(discover_repository_test, repository)
{
    temporary_directory_handle temp_dir;
    auto git = git_invoker(temp_dir);
    git("init");
    ASSERT_TRUE(fs::exists(temp_dir / ".git")) << "Expected git operation to create directory";

    auto start_points = {temp_dir.path(), temp_dir / ".git", temp_dir / "a", temp_dir / "a" / "b"};
    for (const fs::path& start_point : start_points)
    {
        fs::create_directories(start_point); // OK if `start_point` exists.

        auto repo = repository::discover(start_point);
        EXPECT_PATHS_EQUIVALENT(repo.work_directory(), temp_dir);

        auto maybe_repo = repository::try_discover(start_point);
        ASSERT_TRUE(maybe_repo);
        EXPECT_PATHS_EQUIVALENT(maybe_repo->work_directory(), temp_dir);
    }
};

TEST(repository_submodule_paths, submodule_paths)
{
    temporary_directory_handle temp_dir;
    create_directories(temp_dir / "external");

    auto git = git_invoker(temp_dir);
    git("init");
    git("submodule", "add", "--depth", "1", "git://github.com/arsenm/sanitizers-cmake.git",
        "external/sanitizers-cmake");
    git("commit", "-m", "Add submodule");

    repository repo = repository::open(temp_dir);
    auto submodule_paths = repo.submodule_paths();
    ASSERT_EQ(submodule_paths.size(), 1);
    EXPECT_EQ(submodule_paths.front(), "external/sanitizers-cmake");

    // Test nonwork_directories function.
    auto nonwork_paths = nonwork_directories(repo);
    EXPECT_EQ(nonwork_paths.size(), 2);
    EXPECT_PATHS_EQUIVALENT(nonwork_paths[0], temp_dir / ".git");
    EXPECT_PATHS_EQUIVALENT(nonwork_paths[1], temp_dir / "external/sanitizers-cmake");
};

TEST(repository_submodule_paths, codeowners_path)
{
    temporary_directory_handle temp_dir;
    create_directories(temp_dir / "docs");
    create_directories(temp_dir / ".github");

    EXPECT_FALSE(codeowners_path(temp_dir));

    const auto CODEOWNERS = "CODEOWNERS";

    for (const auto& dir : {".github", "docs", "."})
    {
        fs::path p;
        ensure_exists(p = temp_dir / dir / CODEOWNERS);

        auto opt_result = codeowners_path(temp_dir);
        ASSERT_TRUE(opt_result);
        EXPECT_PATHS_EQUIVALENT(*opt_result, p);
    }
};

} // end namespace 'co'
