#include <codeowners/strong_typedef.hpp>
#include <codeowners/type_utils.hpp>

#include <gtest/gtest.h>

#include <memory>

namespace co
{

struct TaggedInt : public strong_typedef<TaggedInt, int>,
                   co::equality_comparable<TaggedInt>,
                   streamable<TaggedInt>
{
    using strong_typedef::strong_typedef;
};

TEST(strong_typedef, operations)
{
    static_assert(std::is_default_constructible_v<TaggedInt>);
    TaggedInt i0{};
    TaggedInt i1{1};
    TaggedInt i2{2};
    EXPECT_EQ(i0.value(), int{0});
    i0.value() = 3;

    EXPECT_FALSE(i1 == i2);
    EXPECT_TRUE(i1 != i2);

    std::stringstream s;
    s << i1;
    ASSERT_TRUE(s);
    EXPECT_EQ(s.str(), "1");
    s >> i2;
    ASSERT_TRUE(s);
    EXPECT_EQ(i2, TaggedInt{1});
}

struct TaggedString : public strong_typedef<TaggedString, std::string>,
                      co::equality_comparable<TaggedString>,
                      streamable<TaggedString>
{
    using strong_typedef::strong_typedef;
};

TEST(strong_typedef, copy_constructor)
{
    TaggedString s0{"zero"};
    TaggedString s1{s0};
    TaggedString s2{std::move(s0)};

    EXPECT_EQ(s1, s2);
}

} // end namespace 'co'
