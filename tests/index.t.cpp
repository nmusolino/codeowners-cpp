#include <codeowners/index.hpp>

#include "tests/test_utils.hpp"
#include <codeowners/repository.hpp>

#include <gtest/gtest.h>

namespace co
{

TEST(index_test, creation)
{
    temporary_directory_handle temp_dir;
    auto git = git_invoker(temp_dir);
    git("init");
    const std::vector<fs::path> filenames{{"a", "b", "c"}};
    for (const auto& filename : filenames)
    {
        ensure_exists(temp_dir / filename);
    }

    {
        repository repo = repository::open(temp_dir);
        index idx = index(repo);
        EXPECT_EQ(idx.begin(), idx.end()) << "Expected empty index before initial commit";
    }

    git("add", ".");
    git("commit", "-m", "Initial commit: add a, b, c");
    git("ls-files");

    {
        repository repo = repository::open(temp_dir);
        index idx = index(repo);
        auto it = idx.begin();
        for (const auto& expected : filenames)
        {
            EXPECT_NE(it, idx.end());
            EXPECT_EQ(*it, expected);
            ++it;
        }
        EXPECT_EQ(it, idx.end());

        std::vector<fs::path> paths{idx.begin(), idx.end()};
        EXPECT_EQ(paths, filenames);
    }
};

} // end namespace 'co'
