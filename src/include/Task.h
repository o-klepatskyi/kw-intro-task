// Copyright (c) Lewis Baker
#pragma once
#include "log.h"

#include <exception>
#include <utility>
#include <type_traits>
#include <cstdint>
#include <cassert>
#include <concepts>
#include <coroutine>

struct broken_promise : std::exception {};

template<typename T> class Task;

class TaskPromiseBase
{
    std::coroutine_handle<> continuation;

    friend struct final_awaitable;

    struct final_awaitable
    {
        bool await_ready() const noexcept { return false; }

        template<typename Promise>
        std::coroutine_handle<> await_suspend(std::coroutine_handle<Promise> coro) noexcept
        {
            return coro.promise().continuation;
        }

        void await_resume() noexcept {}
    };

public:
    TaskPromiseBase() noexcept {}

    auto initial_suspend() noexcept
    {
        return std::suspend_always{};
    }

    auto final_suspend() noexcept
    {
        return final_awaitable{};
    }

    void set_continuation(std::coroutine_handle<> c) noexcept
    {
        continuation = c;
    }
};

template<typename T>
class TaskPromise final : public TaskPromiseBase
{
    enum class ResultType { empty, value, exception };

    ResultType resultType = ResultType::empty;

    union
    {
        T value;
        std::exception_ptr exception;
    };

public:
    TaskPromise() noexcept {}

    ~TaskPromise()
    {
        switch (resultType)
        {
        case ResultType::value:
            value.~T();
            break;
        case ResultType::exception:
            exception.~exception_ptr();
            break;
        default:
            break;
        }
    }

    Task<T> get_return_object() noexcept;

    void unhandled_exception() noexcept
    {
        ::new (static_cast<void*>(std::addressof(exception))) std::exception_ptr(std::current_exception());
        resultType = ResultType::exception;
    }

    template<typename Value>
    requires std::convertible_to<Value&&, T>
    void return_value(Value&& v) noexcept(std::is_nothrow_constructible_v<T, Value&&>)
    {
        ::new (static_cast<void*>(std::addressof(value))) T(std::forward<Value>(v));
        resultType = ResultType::value;
    }

    T& result() &
    {
        if (resultType == ResultType::exception)
        {
            std::rethrow_exception(exception);
        }

        assert(resultType == ResultType::value);

        return value;
    }

    T&& result() &&
    {
        if (resultType == ResultType::exception)
        {
            std::rethrow_exception(exception);
        }

        assert(resultType == ResultType::value);

        return std::move(value);
    }
};

template<>
class TaskPromise<void> : public TaskPromiseBase
{
    std::exception_ptr exception;
public:

    TaskPromise() noexcept = default;

    Task<void> get_return_object() noexcept;

    void return_void() noexcept
    {
        LogInfo("return_void");
    }

    void unhandled_exception() noexcept
    {
        exception = std::current_exception();
    }

    void result()
    {
        if (exception)
        {
            std::rethrow_exception(exception);
        }
    }
};

template<typename T = void>
class Task
{
public:
    using promise_type = TaskPromise<T>;
    using value_type = T;

private:
    using CoroHandle = std::coroutine_handle<promise_type>;
    CoroHandle handle;

    struct awaitable_base
    {
        CoroHandle handle;

        awaitable_base(CoroHandle coroutine) noexcept
        : handle(coroutine)
        {}

        bool await_ready() const noexcept
        {
            return !handle || handle.done();
        }

        std::coroutine_handle<> await_suspend(
            std::coroutine_handle<> awaitingCoroutine) noexcept
        {
            handle.promise().set_continuation(awaitingCoroutine);
            return handle;
        }
    };

public:

    Task() noexcept
    : handle(nullptr)
    {}

    explicit Task(std::coroutine_handle<promise_type> coroutine)
    : handle(coroutine)
    {}

    Task(Task&& t) noexcept
        : handle(t.handle)
    {
        t.handle = nullptr;
    }

    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;

    ~Task()
    {
        if (handle)
        {
            handle.destroy();
        }
    }

    Task& operator=(Task&& other) noexcept
    {
        if (std::addressof(other) != this)
        {
            if (handle)
            {
                handle.destroy();
            }

            handle = other.handle;
            other.handle = nullptr;
        }

        return *this;
    }

    bool ready() const noexcept
    {
        return !handle || handle.done();
    }

    auto operator co_await() const & noexcept
    {
        struct awaitable : awaitable_base
        {
            using awaitable_base::awaitable_base;

            decltype(auto) await_resume()
            {
                if (!this->handle)
                {
                    throw broken_promise{};
                }

                return this->handle.promise().result();
            }
        };

        return awaitable { handle };
    }

    auto operator co_await() const && noexcept
    {
        struct awaitable : awaitable_base
        {
            using awaitable_base::awaitable_base;

            decltype(auto) await_resume()
            {
                if (!this->handle)
                {
                    throw broken_promise{};
                }

                return std::move(this->handle.promise()).result();
            }
        };

        return awaitable { handle };
    }
    
};


template<typename T>
Task<T> TaskPromise<T>::get_return_object() noexcept
{
    return Task<T>{ std::coroutine_handle<TaskPromise>::from_promise(*this) };
}

inline Task<void> TaskPromise<void>::get_return_object() noexcept
{
    return Task<void>{ std::coroutine_handle<TaskPromise>::from_promise(*this) };
}
