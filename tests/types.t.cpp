#include "codeowners/types.hpp"

#include <gtest/gtest.h>

namespace co
{
namespace testing
{

struct X : public strong_typedef<X, int>, equality_comparable<X>
{
    using strong_typedef::strong_typedef;
};

struct Y : public strong_typedef<Y, int>, underlying_equality_comparable<Y, int>
{
    using strong_typedef::strong_typedef;
};

TEST(strong_typedef_test, comparison)
{
    EXPECT_EQ(X(), X(0));
    EXPECT_EQ(X(1), X(1));
    EXPECT_NE(X(0), X(1));
    X x(1);
    EXPECT_EQ(x.value(), 1);
    x.value() = 0;
    EXPECT_EQ(x.value(), 0);
};

TEST(strong_typedef_test, heterogeneous_comparison)
{
    EXPECT_EQ(Y(1), 1);
    EXPECT_EQ(1, Y(1));
    EXPECT_NE(Y(0), 1);
    EXPECT_NE(1, Y(0));
};

}  // end namespace 'testing'
}  // end namespace 'co'