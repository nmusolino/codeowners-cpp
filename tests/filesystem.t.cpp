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
        EXPECT_TRUE(fs::exists(temp_dir.path()));
        EXPECT_TRUE(fs::is_directory(temp_dir.path()));
        temp_dir_path = temp_dir.path();
    }
    EXPECT_FALSE(fs::exists(temp_dir_path)) << "Expected path to be removed: " << temp_dir_path;
};

TEST(temporary_directory_handle_test, path_str)
{
    temporary_directory_handle temp_dir;
    EXPECT_EQ(temp_dir.path_str(), temp_dir.path().string());
};

} /* end namespace 'testing' */
} /* end namespace 'co' */