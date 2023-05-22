#pragma once
#include "util.h"
#include <coroutine>
#include <exception>
#include <iostream>

template<typename T>
class Generator
{
public:
    struct promise_type;
private:
    using Handle = std::coroutine_handle<promise_type>;
    Handle handle;
public:
    Generator(Handle h) noexcept : handle {h}
    {}

    Generator(const Generator&) = delete;

    Generator(Generator&& other) noexcept
    : handle { other.handle }
    {
        other.handle = nullptr;
    }

    ~Generator() noexcept
    {
        if (handle) { handle.destroy(); }
    }

    T value() noexcept
    {
        return handle.promise().currentValue;
    }

    bool next()
    {
        handle.resume();
        return not handle.done();
    }
};

template<typename T>
struct Generator<T>::promise_type
{
    T currentValue;

    using Handle = Generator<T>::Handle;

    auto get_return_object()
    {
        return Handle::from_promise(*this);
    }

    std::suspend_always initial_suspend() noexcept { return {}; }
    std::suspend_always final_suspend()   noexcept { return {}; }

    std::suspend_always yield_value(const T value) noexcept
    {
        currentValue = std::move(value);
        return {};
    }

    void unhandled_exception() noexcept
    {
        std::terminate();
    }

    void return_void() noexcept {}
};

