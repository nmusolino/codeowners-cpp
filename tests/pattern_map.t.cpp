#include <src/pattern_map.hpp>

#include <gtest/gtest.h>

namespace co
{

TEST(pattern_map_test, construction)
{
    pattern_map<int> empty_map;
    EXPECT_TRUE(empty_map.empty());
    EXPECT_EQ(empty_map.size(), 0);

    pattern_map<int> p_map{{pattern{"*.hpp"}, 0}, {pattern{"*.cpp"}, 1}};
    EXPECT_FALSE(p_map.empty());
    EXPECT_EQ(p_map.size(), 2);
}

TEST(pattern_map_test, repeated_insert)
{
    pattern_map<int> p_map;
    pattern pat{"*.hpp"};

    p_map.insert({pat, 1});
    p_map.insert({pat, 2});

    EXPECT_EQ(p_map["sample.hpp"], 1);

    p_map.insert_or_assign(pat, 2);
    EXPECT_EQ(p_map["sample.hpp"], 2);
};

TEST(pattern_map_test, pattern_lookup_overwrite)
{
    pattern_map<int> p_map;
    pattern pat{"*.hpp"};
    p_map.insert({pat, 1});
    p_map[pat] = 2;
    EXPECT_EQ(p_map["sample.hpp"], 2);
};

TEST(pattern_map_test, pattern_insertion_and_path_lookup)
{
    pattern_map<int> p_map;
    p_map[pattern{"*.hpp"}] = 1;
    EXPECT_TRUE(p_map.contains("sample.hpp"));
    EXPECT_EQ(p_map["sample.hpp"], 1);
    EXPECT_EQ(p_map.at("sample.hpp"), 1);
    EXPECT_EQ(p_map.at("sample.hpp"), 1);
    const int* result = p_map.get("sample.hpp");
    EXPECT_EQ(*result, 1);
};

TEST(pattern_map_test, nonmatching_path_lookup)
{
    pattern_map<int> p_map;
    p_map[pattern{"*.hpp"}] = 1;

    const char* const nonmatching = "nonmatching.cpp";
    EXPECT_FALSE(p_map.contains(nonmatching));
    EXPECT_EQ(p_map.find(nonmatching), p_map.end());
    EXPECT_THROW(p_map[nonmatching], std::out_of_range);
    EXPECT_THROW(p_map.at(nonmatching), std::out_of_range);
    EXPECT_EQ(p_map.get(nonmatching), nullptr);
};

TEST(pattern_map_test, contains)
{
    pattern_map<int> p_map;
    pattern pat{"*.hpp"};

    p_map.insert({pat, 1});

    EXPECT_TRUE(p_map.contains(pat));
    EXPECT_TRUE(p_map.contains("sample.hpp"));
    EXPECT_FALSE(p_map.contains("nonmatching.cpp"));
};

} // end namespace 'co'
