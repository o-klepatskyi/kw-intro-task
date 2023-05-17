#pragma once
#include <initializer_list>
#include <utility>
#include <functional>

struct InitializerListTester
{
    bool initListUsed;
    InitializerListTester() : initListUsed {false} {};
    InitializerListTester(const InitializerListTester&) = default;
    InitializerListTester& operator=(const InitializerListTester&) = default;

    InitializerListTester(int, double) : initListUsed {false} {};
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
        return *this;
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

struct Base
{
    mutable int value = 0;
    enum : char {
        BASE_VALUE = 1,
        DERIVED_VALUE
    };

    virtual ~Base() {}

    // is named override to show this is a conditional keyword
    virtual void override()
    {
        value = BASE_VALUE;
    }

    virtual void constQualifier() const
    {
        value = BASE_VALUE;
    }

    virtual void differentParams(int)
    {
        value = BASE_VALUE;
    }
    virtual void refQualifiers() &
    {
        value = BASE_VALUE;
    }
    void nonVirtual() const
    {
        value = BASE_VALUE;
    }
};

struct Derived : public Base
{
    // override is commented to compile
    // but to emphasise we better write it to catch this kinds of errors

    virtual void override() override
    {
        value = DERIVED_VALUE;
    }

    virtual void constQualified() /*override*/
    {
        value = DERIVED_VALUE;
    }

    virtual void differentParams(unsigned int) /*override*/
    {
        value = DERIVED_VALUE;
    }

    virtual void refQualifiers() && /*override*/
    {
        value = DERIVED_VALUE;
    }

    void nonVirtual() const /*override*/
    {
        value = DERIVED_VALUE;
    }
};

template<typename T, typename Func, typename... Args>
inline void invokeMemberFunction(T&& obj, Func func, Args&&... args)
{
    (obj.*func)(std::forward<Args>(args)...);
};

template<typename Func, typename... Args>
inline void invokeBase(Base& obj, Func func, Args&&... args)
{
    invokeMemberFunction(std::forward<Base>(obj), func, std::forward<Args>(args)...);
};

template<typename Func, typename... Args>
inline void invokeBase(Base&& obj, Func func, Args&&... args)
{
    invokeMemberFunction(std::forward<Base>(obj), func, std::forward<Args>(args)...);
};

// TODO: how to rewrite as function template alias???
// template<typename Func, typename... Args>
// using invokeBase = invokeMemberFunction<Base, Func, Args>;

template<typename C>
concept ContiguousContainer = requires(C c) {
    { std::contiguous_iterator<decltype(std::cbegin(c))> };
};

auto findCenter(const ContiguousContainer auto& container)
{
    // we do not modify the values, so use cbegin/cend
    auto left = std::cbegin(container);
    auto right = std::cend(container);
    while (left < right)
    {
        ++left;
        --right;
    };
    return right;
};