#include "codeowners/filesystem.hpp"

#include <gtest/gtest.h>

namespace co
{
namespace testing
{

TEST(ensure_exists_test, creates_file)
{
    temporary_directory_handle temp_dir;
    fs::path path = temp_dir / "file";
    ASSERT_FALSE(fs::exists(path));
    ensure_exists(path);
    EXPECT_TRUE(fs::exists(path));
};

TEST(ensure_exists_test, leaves_existing_file)
{
    temporary_directory_handle temp_dir;

    fs::path path = temp_dir / "file";
    {
        std::ofstream ofs { path.string() };
        ofs << "original_content";
    }
    ASSERT_TRUE(fs::exists(path));

    ensure_exists(path);
    std::ifstream ifs { path.string() };
    std::string contents;
    ifs >> contents;
    EXPECT_EQ(contents, "original_content");
};

TEST(ensure_exists_test, leaves_existing_directory)
{
    temporary_directory_handle temp_dir;
    ASSERT_TRUE(fs::exists(temp_dir));
    ASSERT_TRUE(fs::is_directory(temp_dir));

    ensure_exists(temp_dir);
    EXPECT_TRUE(fs::exists(temp_dir));
    EXPECT_TRUE(fs::is_directory(temp_dir));
};

TEST(temporary_directory_handle_test, construction_and_removal)
{
    fs::path temp_dir_path;
    {
        temporary_directory_handle temp_dir;
        EXPECT_TRUE(fs::exists(temp_dir));
        EXPECT_TRUE(fs::is_directory(temp_dir));
        temp_dir_path = temp_dir.path();
    }
    EXPECT_FALSE(fs::exists(temp_dir_path)) << "Expected path to be removed: " << temp_dir_path;
};

TEST(temporary_directory_handle_test, move_construction)
{
    fs::path temp_dir_path;
    {
        temporary_directory_handle temp_dir;
        ASSERT_TRUE(fs::exists(temp_dir));
        temp_dir_path = temp_dir.path();

        temporary_directory_handle replacement_temp_dir{std::move(temp_dir)};
        EXPECT_TRUE(fs::exists(replacement_temp_dir.path()));
        EXPECT_EQ(replacement_temp_dir.path(), temp_dir_path);
    }
    EXPECT_FALSE(fs::exists(temp_dir_path))
      << "Expected path to be removed after move-construction: "
      << temp_dir_path;
};

TEST(temporary_directory_handle_test, move_assignment)
{
    std::array<fs::path, 2> temp_dir_paths;
    {
        temporary_directory_handle temp_dir1, temp_dir2;
        temp_dir_paths[0] = temp_dir1.path();
        temp_dir_paths[1] = temp_dir2.path();

        EXPECT_NE(temp_dir_paths[0], temp_dir_paths[1]);
        EXPECT_TRUE(fs::exists(temp_dir_paths[0]));
        EXPECT_TRUE(fs::exists(temp_dir_paths[1]));

        temp_dir1 = std::move(temp_dir2);
    }
    EXPECT_FALSE(fs::exists(temp_dir_paths[0]));
    EXPECT_FALSE(fs::exists(temp_dir_paths[1]));
};

TEST(temporary_directory_handle_test, path_str)
{
    temporary_directory_handle temp_dir;
    EXPECT_EQ(temp_dir.path_str(), temp_dir.path().string());
};

} /* end namespace 'testing' */
} /* end namespace 'co' */