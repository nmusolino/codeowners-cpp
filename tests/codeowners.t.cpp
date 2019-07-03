#include "codeowners/codeowners.hpp"

#include <gtest/gtest.h>


namespace co {
namespace testing {

    TEST(MessageTest, EmptyString)
    {
        EXPECT_EQ(message(std::string{}), "Hello");
    }

    TEST(MessageTest, NonemptyString)
    {
        EXPECT_EQ(message("World"), "Hello, World");
    }

} /* end namespace 'testing' */
} /* end namespace 'co' */
