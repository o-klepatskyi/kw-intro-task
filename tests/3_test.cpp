#include "gtest/gtest.h"
#include "3_moving_to_modern_cpp.h"
#include <memory>
#include <concepts>
#include <vector>
#include <array>
#include <string>
#include <thread>
#include <chrono>
#include <future>
#include <functional>

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

TEST(Item16, ReturnsWrongValueWithoutMutex)
{
    const int base = 2;
    const int power = 5;
    const CashedPower cpower {base, power};
    const int expected = std::pow(base, power);
    std::promise<int> firstPromise;
    std::promise<int> secondPromise;

    std::thread([&firstPromise, &cpower]{
        int result = cpower.notMultithread1();
        firstPromise.set_value(result);
    }).detach();

    std::thread([&secondPromise, &cpower]{
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        int result = cpower.notMultithread1();
        secondPromise.set_value(result);
    }).detach();

    int firstResult = firstPromise.get_future().get();
    int secondResult = secondPromise.get_future().get();

    EXPECT_EQ(cpower.timesComputed(), 1);
    EXPECT_EQ(firstResult, expected);
    EXPECT_EQ(secondResult, 0);
}

TEST(Item16, ComputesTwiceWithoutMutex)
{
    const int base = 2;
    const int power = 5;
    const CashedPower cpower {base, power};
    const int expected = std::pow(base, power);
    std::promise<int> firstPromise;
    std::promise<int> secondPromise;

    std::thread([&firstPromise, &cpower]{
        int result = cpower.notMultithread2();
        firstPromise.set_value(result);
    }).detach();

    std::thread([&secondPromise, &cpower]{
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        int result = cpower.notMultithread2();
        secondPromise.set_value(result);
    }).detach();

    int firstResult = firstPromise.get_future().get();
    int secondResult = secondPromise.get_future().get();

    EXPECT_EQ(cpower.timesComputed(), 2);
    EXPECT_EQ(firstResult, expected);
    EXPECT_EQ(secondResult, expected);
}

TEST(Item16, WorksCorrectlyWithMutex)
{
    const int base = 2;
    const int power = 5;
    const CashedPower cpower {base, power};
    const int expected = std::pow(base, power);
    std::promise<int> firstPromise;
    std::promise<int> secondPromise;

    std::thread([&firstPromise, &cpower]{
        int result = cpower.multithread();
        firstPromise.set_value(result);
    }).detach();

    std::thread([&secondPromise, &cpower]{
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        int result = cpower.multithread();
        secondPromise.set_value(result);
    }).detach();

    int firstResult = firstPromise.get_future().get();
    int secondResult = secondPromise.get_future().get();

    EXPECT_EQ(cpower.timesComputed(), 1);
    EXPECT_EQ(firstResult, expected);
    EXPECT_EQ(secondResult, expected);
}

TEST(Item17, DefinesCopyAssignmentIfCopyCtorIsUserDefined)
{
    CopyableConstructor c1;
    c1.value = 5; 
    CopyableConstructor c2;
    EXPECT_NE(c1.value, c2.value);
    c2 = c1; // generates default copy
    EXPECT_EQ(c1.value, c2.value);
}

TEST(Item17, DefinesCopyCtorIfCopyAssignmentIsUserDefined)
{
    CopyableAssignment c1;
    c1.value = 5;
    CopyableAssignment c2 {c1}; // generates default copy
    EXPECT_EQ(c1.value, c2.value);
}

TEST(Item17, DontDefineMoveAssignmentIfMoveCtorIsDefined)
{
    MovableConstuctor c1;
    MovableConstuctor c2 {std::move(c1)};
    // MovableConstuctor c2 {c1}; copy operations are deleted
    // c2 = c1;                   if move are defined
    
    // c2 = std::move(c1); <-- won't compile
    //                         because move ctor is defined
}

TEST(Item17, DontDefineMoveCtorIfMoveAssignmentIsDefined)
{
    MovableAssignment c1, c2;
    // MovableConstuctor c2 {c1}; same story
    // c2 = c1;                   
    c2 = std::move(c1);
    
   
    //  MovableAssignment c3 {c2}; <-- won't compile
    //                                 because move assignment is defined
}