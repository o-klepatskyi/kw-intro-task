#include "gtest/gtest.h"
#include "3_moving_to_modern_cpp.h"
#include <memory>
#include <concepts>

using ILT = InitializerListTester;

TEST(Item7, BraceInitializationImplicitConvertsToWiderType)
{
    long long ll {5};
    EXPECT_EQ(5L, ll);
}

TEST(Item7, ParensUsesCtorWithTwoParams)
{
    ILT obj (10, 5.0);
    EXPECT_FALSE(obj.initListUsed);
}

TEST(Item7, BracesUsesCtorWithInitList)
{
    ILT obj {10, 5.0};
    EXPECT_TRUE(obj.initListUsed);
}

TEST(Item7, EmptyBracesCallDefaultCtor)
{
    ILT obj {};
    EXPECT_FALSE(obj.initListUsed);
}

TEST(Item7, NestedBracesCalCtorWithEmptyInitList)
{
    ILT obj {{}};
    EXPECT_TRUE(obj.initListUsed);
}

TEST(Item7, TemplateReturnBraceInitCallsCtorWithInitList)
{
    ILT obj = bracesReturnConstruct(1.0, 5.0);
    EXPECT_TRUE(obj.initListUsed);
}

TEST(Item7, TemplateLocalBraceInitCallsCtorWithInitList)
{
    ILT obj = bracesLocalConstruct(1.0, 5.0);
    EXPECT_TRUE(obj.initListUsed);
}

TEST(Item7, TemplateLocalBraceInitCallsCtorWithTwoParams)
{
    ILT obj = parensLocalConstruct(1.0, 5.0);
    EXPECT_FALSE(obj.initListUsed);
}

TEST(Item8, NULLCallsIntCtor)
{
    IntCopyer obj(NULL);
    // NULL is defined as int, so value is copied
    EXPECT_EQ(obj.value, 0);
}

TEST(Item8, NullptrCallsIntPointerCtor)
{
    IntCopyer obj(nullptr);
    // nullptr -> int*, thus -1
    EXPECT_EQ(obj.value, -1);
}

TEST(Item8, ValueOfPointerIsCopiedCorrectly)
{
    const int expected = 5;
    auto ptr = std::make_unique<int>(expected);
    IntCopyer obj(ptr.get());
    // just a copy of value
    EXPECT_EQ(obj.value, expected);
}

TEST(Item9, FunctionPtrTypedefIsSameAsAlias)
{
    static_assert(std::same_as<TypedefFunctionReturnsInt, AliasFunctionReturnsInt>);
}

TEST(Item9, TypenameClassMemberAndTemplateAliasAreSame)
{
    static_assert(std::same_as<OneTemplateParamTypename<int>::type, OneTemplateParamAlias<int>>);
}

TEST(Item10, UnscopedEnumCanBeUsedWithAndWithoutEnumName)
{
    EXPECT_EQ(PollutedNamespace::FIRST, PollutedNamespace::UnscopedEnum::FIRST);
}

TEST(Item10, ScopedEnumProhibitsImplicitConversion)
{
    using namespace PollutedNamespace;
    auto scopedValue = enumToNumber<UnscopedEnum::THIRD>();
    auto unscopedValue = enumToNumber<static_cast<int>(ScopedEnum::THIRD)>();
    EXPECT_EQ(scopedValue, unscopedValue);
}

TEST(Item11, DeletedCopyCtorCannotBePassedByValue)
{
    const int expected = 5;
    MovableOnly obj {expected};
    // won't compile - we need a cast to rvalue
    // auto value = movableConsumer(obj);
    
    // move doesn't move anything, just performs casting to rvalue
    auto value = movableConsumer(std::move(obj));
    EXPECT_EQ(expected, value);
    EXPECT_EQ(0, obj.value);
}

TEST(Item11, CannotCallDeletedWithNullptr)
{
    auto expected {5};
    auto ptr = std::make_unique<int>(expected);
    auto val = dereferenceAndRejectNullptr(ptr.get());
    EXPECT_EQ(expected, val);
    
    // would not compile!
    //dereferenceAndRejectNullptr(nullptr);
}