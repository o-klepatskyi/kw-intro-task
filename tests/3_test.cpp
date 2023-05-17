#include "gtest/gtest.h"
#include "3_moving_to_modern_cpp.h"
#include <memory>

using ILT = InitializerListTester;

TEST(Item8, BraceInitializationImplicitConvertsToWiderType)
{
    long long ll {5};
    EXPECT_EQ(5L, ll);
}

TEST(Item8, ParensUsesCtorWithTwoParams)
{
    ILT obj (10, 5.0);
    EXPECT_FALSE(obj.initListUsed);
}

TEST(Item8, BracesUsesCtorWithInitList)
{
    ILT obj {10, 5.0};
    EXPECT_TRUE(obj.initListUsed);
}

TEST(Item8, EmptyBracesCallDefaultCtor)
{
    ILT obj {};
    EXPECT_FALSE(obj.initListUsed);
}

TEST(Item8, NestedBracesCalCtorWithEmptyInitList)
{
    ILT obj {{}};
    EXPECT_TRUE(obj.initListUsed);
}

TEST(Item8, TemplateReturnBraceInitCallsCtorWithInitList)
{
    ILT obj = bracesReturnConstruct(1.0, 5.0);
    EXPECT_TRUE(obj.initListUsed);
}

TEST(Item8, TemplateLocalBraceInitCallsCtorWithInitList)
{
    ILT obj = bracesLocalConstruct(1.0, 5.0);
    EXPECT_TRUE(obj.initListUsed);
}

TEST(Item8, TemplateLocalBraceInitCallsCtorWithTwoParams)
{
    ILT obj = parensLocalConstruct(1.0, 5.0);
    EXPECT_FALSE(obj.initListUsed);
}

TEST(Item9, NULLCallsIntCtor)
{
    IntCopyer obj(NULL);
    // NULL is defined as int, so value is copied
    EXPECT_EQ(obj.value, 0);
}

TEST(Item9, NullptrCallsIntPointerCtor)
{
    IntCopyer obj(nullptr);
    // nullptr -> int*, thus -1
    EXPECT_EQ(obj.value, -1);
}

TEST(Item9, ValueOfPointerIsCopiedCorrectly)
{
    const int expected = 5;
    auto ptr = std::make_unique<int>(expected);
    IntCopyer obj(ptr.get());
    // just a copy of value
    EXPECT_EQ(obj.value, expected);
}