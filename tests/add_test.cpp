#include <gtest/gtest.h>
#include <addition/add.h>

TEST(AddTest, AdditionTest)
{
    const auto expected = 2;
    const auto actual   = add(1, 1);
    ASSERT_EQ(expected, actual);
}
