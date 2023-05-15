#include <gtest/gtest.h>
#include "2_auto.h"
#include <vector>
#include <list>
#include <numeric>

TEST(C2_auto, AutoDeducesInnerTypename)
{
    Outer outer;
    auto t = outer.getInner();
    EXPECT_TRUE(CheckInner(t).value); 
}

TEST(C2_auto, UsingExplicitlyTypedInitializerIdiom)
{
    Outer outer;
    auto i = static_cast<int>(outer.getInner());
    EXPECT_FALSE(CheckInner(i).value); 
}

TEST(C2_auto, UsingAutoForVectorSum)
{
    std::vector<int> vec = { 1, 2, 3, 4, 5, -20, -15, -10, 10, 30, 40};
    auto vecSum = autoSum(vec);
    auto actual = std::accumulate(vec.begin(), vec.end(), 0);
    EXPECT_EQ(actual, vecSum);
}

TEST(C2_auto, UsingAutoForListSum)
{
    std::list<int> lis = { 1, 2, 3, 4, 5, -20, -15, -10, 10, 30, 40};
    auto listSum = autoSum(lis);
    auto actual = std::accumulate(lis.begin(), lis.end(), 0);
    EXPECT_EQ(actual, listSum);
}