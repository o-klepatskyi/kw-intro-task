#include "gtest/gtest.h"
#include "3_moving_to_modern_cpp.h"
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
    ILT obj = bracesReturnConstruct(1, 5.0);
    EXPECT_TRUE(obj.initListUsed);
}

TEST(Item8, TemplateLocalBraceInitCallsCtorWithInitList)
{
    ILT obj = bracesLocalConstruct(1, 5.0);
    EXPECT_TRUE(obj.initListUsed);
}

TEST(Item8, TemplateLocalBraceInitCallsCtorWithTwoParams)
{
    ILT obj = parensLocalConstruct(1, 5.0);
    EXPECT_FALSE(obj.initListUsed);
}