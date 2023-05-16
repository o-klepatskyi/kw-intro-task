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