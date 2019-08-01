#include "codeowners/filesystem.hpp"

#include <gtest/gtest.h>

namespace co
{
namespace testing
{

TEST(temporary_directory_handle_test, construction)
{

    temporary_directory_handle::path_type path;
    {
        temporary_directory_handle handle;
        path = handle.path();
        EXPECT_TRUE(fs::exists(path));
        EXPECT_TRUE(fs::is_directory(path));
    }
    EXPECT_FALSE(fs::exists(path));
};

TEST(temporary_directory_handle_test, path_str)
{
    temporary_directory_handle handle;
    EXPECT_EQ(handle.path().string(), handle.path_str());
};

} /* end namespace 'testing' */
} /* end namespace 'co' */