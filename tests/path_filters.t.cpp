#include "codeowners/filesystem.hpp"
#include "codeowners/path_filters.hpp"

#include <gtest/gtest.h>

namespace co
{

TEST(filter_iterator, skips_git_directory)
{
    temporary_directory_handle temp_dir;

    // Create a tree like this:
    //      temp_dir/a/
    //      temp_dir/b/
    //      temp_dir/x/
    //      temp_dir/x/subdir/
    //
    // When the filter iterator is constructed using suffix `x`,
    // only the first two entries should be presented.
    for (const auto& subdirectory : {"a", "b", "x"})
    {
        fs::create_directories(temp_dir / subdirectory);
    }
    fs::create_directories(temp_dir / "x" / "subdir");

    auto begin = recursive_filter_iterator{"x", temp_dir};
    auto end = recursive_filter_iterator{};

    std::vector<fs::directory_entry> traversed(begin, end);
    EXPECT_EQ(traversed.size(), 2);
    EXPECT_EQ(traversed[0].path().filename(), "a");
    EXPECT_EQ(traversed[1].path().filename(), "b");
};

} /* end namespace 'co' */