#include <codeowners/errors.hpp>
#include <codeowners/filesystem.hpp>
#include <codeowners/repository.hpp>

#include <gtest/gtest.h>

#include <boost/process.hpp>

namespace co
{
namespace testing
{

struct git_invoker
{
    git_invoker(fs::path repository_root)
      : m_repository_root{std::move(repository_root)} {};

    template <typename... Args>
    void operator()(Args&&... args)
    {
        boost::process::system(boost::process::search_path("git"),
                               std::forward<Args>(args)...,
                               boost::process::start_dir =
                                 m_repository_root.string(),
                               boost::process::std_out > boost::process::null,
                               boost::process::std_err > boost::process::null,
                               boost::process::throw_on_error);
    }

private:
    fs::path m_repository_root;
};

/// Return whether the two paths are equivalent, i.e. refer to the same
/// filesystem entity.
::testing::AssertionResult
equivalent(const fs::path& p1, const fs::path& p2)
{
    // NOTE: Google Test bug #1614 applies when printing two fs::path objects.
    // https://github.com/google/googletest/issues/1614
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

#define EXPECT_PATHS_EQUIVALENT(p1, p2)                                        \
    EXPECT_TRUE(co::testing::equivalent((p1), (p2)))

TEST(discover_repository_test, no_repository)
{
    temporary_directory_handle temp_dir;

    // When a directory does not contain a git repository, attempting to
    // discover a repository fails.
    EXPECT_THROW(repository::discover(temp_dir),
                 co::repository_not_found_error);
    std::optional<repository> result = repository::try_discover(temp_dir);
    EXPECT_FALSE(result);

    // When a directory does not exist, both `discover` and `try_discover`
    // will raise an exception.
    EXPECT_THROW(repository::discover(temp_dir / "nonexistent_dir"),
                 co::file_not_found_error);
    EXPECT_THROW(repository::try_discover(temp_dir / "nonexistent_dir"),
                 co::file_not_found_error);
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
    ASSERT_TRUE(fs::exists(temp_dir / ".git"))
      << "Expected git operation to create directory";

    auto start_points = {
      temp_dir.path(), temp_dir / ".git", temp_dir / "a", temp_dir / "a" / "b"};
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
    git("submodule",
        "add",
        "--depth",
        "1",
        "git://github.com/arsenm/sanitizers-cmake.git",
        "external/sanitizers-cmake");
    git("commit", "-m", "Add submodule");

    repository repo = repository::open(temp_dir);
    auto submodule_paths = repo.submodule_paths();
    ASSERT_EQ(submodule_paths.size(), 1);
    EXPECT_EQ(submodule_paths.front(), "external/sanitizers-cmake");
};

} // end namespace 'testing'
} // end namespace 'co'
