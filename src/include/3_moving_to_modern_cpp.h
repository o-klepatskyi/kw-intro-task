#pragma once
#include <initializer_list>
#include <utility>

struct InitializerListTester
{
    bool initListUsed;
    InitializerListTester() : initListUsed {false} {};
    InitializerListTester(const InitializerListTester&) = default;
    InitializerListTester& operator=(const InitializerListTester&) = default;

    InitializerListTester(int i, double b) : initListUsed {false} {};
    InitializerListTester(std::initializer_list<double>) : initListUsed {true} {};
};

template<typename... Ps>
InitializerListTester bracesReturnConstruct(Ps&&... params)
{
    return {std::forward<Ps>(params)...};
};

template<typename... Ps>
InitializerListTester parensLocalConstruct(Ps&&... params)
{
    InitializerListTester local (std::forward<Ps>(params)...);
    return local;
};

template<typename... Ps>
InitializerListTester bracesLocalConstruct(Ps&&... params)
{
    InitializerListTester local {std::forward<Ps>(params)...};
    return local;
};

struct IntCopyer
{
    int value;
    IntCopyer(int i) : value {i} {}

    IntCopyer(int* pi) : value {-1}
    {
        if (pi) value = *pi;
    }
};

typedef int (*TypedefFunctionReturnsInt)();
using AliasFunctionReturnsInt = int (*)();

template<typename F, typename S>
struct TwoTemplateParams {};

template<typename F>
struct OneTemplateParamTypename
{
    typedef TwoTemplateParams<F, void> type;
};

template <typename F>
using OneTemplateParamAlias = TwoTemplateParams<F, void>;

namespace PollutedNamespace
{
    enum UnscopedEnum : char
    {
        FIRST = 1,
        SECOND,
        THIRD
    };

    enum class ScopedEnum : int
    {
        SECOND = 2,
        THIRD,
        FOURTH
    };
};

template<long long i>
constexpr long long enumToNumber()
{
    return i;
};

struct MovableOnly
{
    int value = 0;

    MovableOnly() = default;
    MovableOnly(int a) : value {a} {}
    MovableOnly(const MovableOnly&) = delete;
    MovableOnly& operator=(const MovableOnly&) = delete;
    MovableOnly(MovableOnly&& rhs) noexcept 
    : value {rhs.value}
    {
        rhs.value = 0;
    }
    // rule of 5 tells us to define move assign too
    MovableOnly& operator=(MovableOnly&& rhs) noexcept
    {
        value = rhs.value;
        rhs.value = 0;
    }
};

int movableConsumer(MovableOnly obj)
{
    return obj.value;
}

template<typename T>
constexpr T dereferenceAndRejectNullptr(T* ptr)
{
    return *ptr;
}

void dereferenceAndRejectNullptr(std::nullptr_t) = delete;