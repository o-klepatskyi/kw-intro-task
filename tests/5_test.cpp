#include "gtest/gtest.h"
#include "5_move_semantics.h"
#include <string>

using MyConsumer = AnyConsumer<std::string, int>;

TEST(C5_move_semantics, CorrectlyForwardLValue)
{
    char a = 'a';
    auto res = consume(a);
    EXPECT_EQ(res, Consumed::LVALUE);
}

TEST(C5_move_semantics, CorrectlyForwardRValue)
{
    char a = 'a';
    auto res = consume(std::move(a));
    EXPECT_EQ(res, Consumed::RVALUE);
    res = consume(Gadget());
    EXPECT_EQ(res, Consumed::RVALUE);
}

TEST(C5_move_semantics, CallsOverridenFunctionForInt)
{
    auto res = overrideForInts(5);
    EXPECT_EQ(res, Consumed::INT);
}

TEST(C5_move_semantics, CorrectlyCopiesAnyConsumer)
{
    MyConsumer cons1 {5};
    MyConsumer cons2 {cons1};

    EXPECT_EQ(cons1.value, cons2.value);
}

TEST(C5_move_semantics, CorrectlyMovesAnyConsumer)
{
    MyConsumer cons1 {"my string"};
    MyConsumer cons2 {std::move(cons1)};
    MyConsumer empty {};

    EXPECT_NE(cons1.value, cons2.value);
    EXPECT_EQ(cons1.value, empty.value);
}