#pragma once
#include <type_traits>

struct A
{
    bool copied = false;
    mutable bool constcopied = false;

    int i = 0;
    mutable int m = 0;

    A() = default;

    A(A& a)
    {
        a.copied = true;
    }

    A(const A& a)
    {
        a.constcopied = true;
    }

    A& increment()
    {
        i++;
        return *this;
    }

    const A& increment() const
    {
        m++;
        return *this;
    }
};

template <typename T>
void ref(T& obj)
{
    obj.increment();
}

template <typename T>
void uref(T&& obj)
{
    obj.increment();
}

template<typename T>
void val(T obj)
{
    if constexpr(std::is_pointer_v<T>) {
        obj->increment();
    } else {
        obj.increment();
    }
}

template<typename T>
auto autoReturn(T& obj)
{
    return obj.increment();
}

template<typename T>
decltype(auto) decltypeAutoReturn(T& obj)
{
    return obj.increment();
}