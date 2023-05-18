#include "gtest/gtest.h"
#include "3_moving_to_modern_cpp.h"
#include <memory>
#include <concepts>
#include <vector>
#include <array>
#include <string>

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

TEST(Item12, InvokeBaseTemplateInvokesCorrectFunction)
{
    Base base;
    Derived derived;
    invokeBase(base, &Base::override);
    invokeBase(derived, &Base::override);
    EXPECT_EQ(base.value, Base::BASE_VALUE);
    EXPECT_EQ(derived.value, Base::DERIVED_VALUE);
}

TEST(Item12, FuncIsNotOverridedWithDifferentConstQual)
{
    Base base;
    Derived derived;
    invokeBase(base, &Base::constQualifier);
    invokeBase(derived, &Base::constQualifier);
    EXPECT_EQ(base.value, Base::BASE_VALUE);
    EXPECT_EQ(derived.value, Base::BASE_VALUE);
}

TEST(Item12, FuncIsNotOverridedWithDifferentParams)
{
    Base base;
    Derived derived;
    invokeBase(base, &Base::differentParams, 0);
    invokeBase(derived, &Base::differentParams, 0);
    EXPECT_EQ(base.value, Base::BASE_VALUE);
    EXPECT_EQ(derived.value, Base::BASE_VALUE);
}

TEST(Item12, FuncIsNotOverridedWithRefQualifiers)
{
    Base base;
    Derived derived;
    invokeBase(base, &Base::refQualifiers);
    invokeBase(std::move(derived), &Base::refQualifiers);
    EXPECT_EQ(base.value, Base::BASE_VALUE);
    EXPECT_EQ(derived.value, Base::BASE_VALUE);
}

TEST(Item12, FuncIsNotOverridedIfNotMarkedVirtualInBaseClass)
{
    Base base;
    Derived derived;
    invokeBase(base, &Base::nonVirtual);
    invokeBase(std::move(derived), &Base::nonVirtual);
    EXPECT_EQ(base.value, Base::BASE_VALUE);
    EXPECT_EQ(derived.value, Base::BASE_VALUE);
}

TEST(Item13, EmptyContainerCenterIsEnd)
{
    std::vector<int> container;
    auto center = findCenter(container);
    EXPECT_EQ(center, container.cend());
}

TEST(Item13, FindsCorrectCenterWithOddElements)
{
    std::string container {"abcde"};
    auto center = findCenter(container);
    auto expected = std::next(std::cbegin(container), container.size() / 2);
    EXPECT_EQ(center, expected);
}

TEST(Item13, FindsCorrectCenterWithEvenElements)
{
    std::array<int, 6> container {1,2,3,4,5,6};
    auto center = findCenter(container);
    auto expected = std::next(std::cbegin(container), container.size() / 2);
    EXPECT_EQ(center, expected);
}

TEST(Item15, CreatesObjectAtCompileTime)
{
    constexpr CompileTimeWidget w2 {5};
    //w2.set(10); <---- const qualifier needed. Why?
    static_assert(5 == w2.get());
}

TEST(Item15, DoesCalculationsAtCompileTime)
{
    constexpr CompileTimeWidget w { gcd(36, 48) };
    constexpr auto expected = 12;
    static_assert(expected == w.get());
}