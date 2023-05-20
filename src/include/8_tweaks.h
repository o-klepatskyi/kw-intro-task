#pragma once
#include <utility>
#include <concepts>

struct CopyCounter
{
    size_t copy = 0;
    size_t move = 0;

    CopyCounter() = default;
    CopyCounter(int /*not used*/) {}

    CopyCounter(const CopyCounter& other) noexcept
    : copy {other.copy + 1}, move {other.move}
    {}

    CopyCounter& operator=(const CopyCounter& other) noexcept
    {
        copy = other.copy + 1;
        move = other.move;
        return *this;
    }

    CopyCounter(CopyCounter&& other) noexcept
    : copy {other.copy}, move {other.move + 1}
    {}

    CopyCounter& operator=(CopyCounter&& other)
    {
        copy = other.copy;
        move = other.move + 1;
    }
};

struct ByCopyAndMove
{
    CopyCounter value;

    ByCopyAndMove(const CopyCounter& c) : value {c} {}
    ByCopyAndMove(CopyCounter&& c) : value { std::move(c) } {}

};

struct ByUniversalReference
{
    CopyCounter value;

    template<typename T>
    requires std::same_as<std::decay_t<T>, CopyCounter>
    ByUniversalReference(T&& c) : value { std::forward<T>(c) } {}
};

struct ByValue
{
    CopyCounter value;

    ByValue(CopyCounter c) : value { std::move(c) } {}
};