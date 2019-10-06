#include <src/attribute_set.hpp>

#include <gtest/gtest.h>

namespace co
{

TEST(attribute_set_test, pattern)
{
    attribute_set attr_set;
    attr_set.add_pattern(pattern{"*.hpp"}, "x");
    attr_set.add_pattern(pattern{"*.cpp"}, "y");

    EXPECT_EQ(attr_set.get("sample.hpp"), "x");
    EXPECT_EQ(attr_set.get("sample.cpp"), "y");
};

TEST(attribute_set_test, undefined_value)
{
    attribute_set attr_set;
    attr_set.add_pattern(pattern{"*.hpp"}, "foo");

    EXPECT_FALSE(bool(attr_set.get_optional("unmatched_file")));
    EXPECT_THROW(attr_set.get("unmatched_file"),
                 attribute_set::no_attribute_error);
};

TEST(attribute_set_test, attribute_name)
{
    attribute_set attr_set{"my_attr_name"};
    EXPECT_EQ(attr_set.attribute_name(), "my_attr_name");
};

TEST(attribute_set_test, write)
{
    attribute_set attr_set{"my_attr",
                           {{pattern{"*.hpp"}, "x"}, {pattern{"*.cpp"}, "y"}}};
    std::ostringstream os;
    attr_set.write(os);
    EXPECT_EQ(os.str(),
              "*.hpp    my_attr=x\n"
              "*.cpp    my_attr=y\n");
};

TEST(attribute_set_test, swap)
{
    attribute_set attr_set1;
    attr_set1.add_pattern(pattern{"*.hpp"}, "x");
    attribute_set attr_set2;
    attr_set2.add_pattern(pattern{"*.hpp"}, "y");

    EXPECT_EQ(attr_set1.get("sample.hpp"), "x");
    EXPECT_EQ(attr_set2.get("sample.hpp"), "y");

    attr_set1.swap(attr_set2);
    EXPECT_EQ(attr_set1.get("sample.hpp"), "y");
    EXPECT_EQ(attr_set2.get("sample.hpp"), "x");
};

} // end namespace 'co'
