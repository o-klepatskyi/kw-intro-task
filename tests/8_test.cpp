#include "8_tweaks.h"
#include "gtest/gtest.h"
#include <vector>

TEST(C8_tweaks, CopyingByLValueRefIsOneCopy)
{
    CopyCounter c;
    ByCopyAndMove consumer {c};
    EXPECT_EQ(consumer.value.copy, 1);
    EXPECT_EQ(consumer.value.move, 0);
}

TEST(C8_tweaks, CopyingByRValueRefIsAlwaysOneMove)
{
    ByCopyAndMove consumer {CopyCounter()};
    EXPECT_EQ(consumer.value.copy, 0);
    EXPECT_EQ(consumer.value.move, 1);
}

TEST(C8_tweaks, CopyingLValueByUniversalRefIsAlwaysOneCopy)
{
    CopyCounter c;
    ByUniversalReference consumer {c};
    EXPECT_EQ(consumer.value.copy, 1);
    EXPECT_EQ(consumer.value.move, 0);
}

TEST(C8_tweaks, CopyingRValueByUniversalRefIsAlwaysOneMove)
{
    ByUniversalReference consumer {CopyCounter()};
    EXPECT_EQ(consumer.value.copy, 0);
    EXPECT_EQ(consumer.value.move, 1);
}

TEST(C8_tweaks, CopyingLValueByValueIsCopyAndMove)
{
    CopyCounter c;
    ByValue consumer {c};
    EXPECT_EQ(consumer.value.copy, 1);
    EXPECT_EQ(consumer.value.move, 1);
}

TEST(C8_tweaks, CopyingRValueByValueIsTwoMoves)
{
    CopyCounter c;
    ByValue consumer {std::move(c)};
    EXPECT_EQ(consumer.value.copy, 0);
    EXPECT_EQ(consumer.value.move, 2);
}

TEST(C8_tweaks, RValueIsConstructedInPlaceWhenPassedByValue)
{
    ByValue consumer {CopyCounter()};
    EXPECT_EQ(consumer.value.copy, 0);
    EXPECT_EQ(consumer.value.move, 1); // <- thus only one move
}

TEST(C8_tweaks, PushAndEmplaceIsOneCopyIfWholeObjectIsPassed)
{
    CopyCounter c1;
    std::vector<CopyCounter> vec;
    vec.reserve(2); // so it doesn't resize and objects are not moved
    vec.push_back(c1);
    vec.emplace_back(c1);
    EXPECT_EQ(vec[0].copy, 1);
    EXPECT_EQ(vec[0].move, 0);
    EXPECT_EQ(vec[1].copy, 1);
    EXPECT_EQ(vec[1].move, 0);
}

TEST(C8_tweaks, EmplacementCostsOnlyOneConstructor)
{
    std::vector<CopyCounter> vec;
    vec.emplace_back(5);
    EXPECT_EQ(vec[0].copy, 0);
    EXPECT_EQ(vec[0].move, 0);
}