#include "codeowners/filesystem.hpp"
#include "codeowners/path_sources.hpp"

#include <gtest/gtest.h>

namespace co
{

/**
 * Create the directory hierarchy depicted below.
 *
 *     temp_dir/a/
 *     temp_dir/b/
 *     temp_dir/x/
 *     temp_dir/x/subdir/
 */
temporary_directory_handle create_hierarchy()
{
    temporary_directory_handle temp_dir;

    // When the filter iterator is constructed using suffix `x`,
    // only the first two entries should be presented.
    for (const auto& subdirectory : {"a", "b", "x", "x/subdir"})
    {
        fs::create_directories(temp_dir / subdirectory);
    }
    fs::create_directories(temp_dir);
    return temp_dir;
}

TEST(filter_iterator, skips_none_or_all)
{
    temporary_directory_handle temp_dir{create_hierarchy()};

    {
        auto true_pred = [](const fs::path&) { return true; };
        std::vector<fs::directory_entry> traversed(recursive_filter_iterator{temp_dir, true_pred},
                                                   recursive_filter_iterator{});
        EXPECT_EQ(traversed.size(), 4);
    }
    {
        auto false_pred = [](const fs::path&) { return false; };
        std::vector<fs::directory_entry> traversed(recursive_filter_iterator{temp_dir, false_pred},
                                                   recursive_filter_iterator{});
        EXPECT_EQ(traversed.size(), 0);
    }
}

TEST(filter_iterator, skips_indicated_directory)
{
    temporary_directory_handle temp_dir{create_hierarchy()};

    auto not_x_pred = [](const fs::path& p) { return p.filename() != "x"; };
    auto begin = recursive_filter_iterator{temp_dir, not_x_pred};
    auto end = recursive_filter_iterator{};

    std::vector<fs::directory_entry> result(begin, end);
    ASSERT_EQ(result.size(), 2);
    EXPECT_EQ(fs::relative(result[0], temp_dir).string(), "a");
    EXPECT_EQ(fs::relative(result[1], temp_dir).string(), "b");
};

TEST(filter_iterator, skip_multiple_directories)
{
    temporary_directory_handle temp_dir{create_hierarchy()};

    auto to_skip = std::vector<fs::path>{temp_dir / "a", temp_dir / "b"};
    auto result = make_filtered_file_range(temp_dir, to_skip) | ranges::to<std::vector<fs::path>>();

    ASSERT_EQ(result.size(), 2);
    EXPECT_EQ(fs::relative(result[0], temp_dir).string(), "x");
    EXPECT_EQ(fs::relative(result[1], temp_dir).string(), "x/subdir");
};

} /* end namespace 'co' */