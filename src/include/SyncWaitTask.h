#pragma once
#include "Task.h"

#include <coroutine>
#include <utility>
#include <type_traits>
#include <future>

template<typename T>
class SyncWaitTask;

template<typename T>
class SyncWaitTaskPromise final
{
    using CoroHandle = std::coroutine_handle<SyncWaitTaskPromise<T>>;

    std::remove_reference_t<T> resultValue;
    std::exception_ptr exception;
public:

    SyncWaitTask<T> get_return_object() noexcept
    {
        return { CoroHandle::from_promise(*this) };
    }

    std::suspend_never initial_suspend() noexcept
    {
        return {};
    }

    std::suspend_always final_suspend() noexcept
    {
        return {};
    }

    std::suspend_always yield_value(T&& result) noexcept(std::is_nothrow_move_constructible_v<T>)
    {
        resultValue = std::move(result);
        return {};
    }

    void return_void() noexcept
    {
        // The coroutine should have either yielded a value or thrown
        // an exception in which case it should have bypassed return_void().
        assert(false);
    }

    void unhandled_exception()
    {
        exception = std::current_exception();
    }

    T&& result()
    {
        if (exception)
        {
            std::rethrow_exception(exception);
        }

        return std::move(resultValue);
    }
};

template<>
class SyncWaitTaskPromise<void>
{
    using CoroHandle = std::coroutine_handle<SyncWaitTaskPromise<void>>;

    std::exception_ptr exception;
public:

    auto get_return_object() noexcept
    {
        return CoroHandle::from_promise(*this);
    }

    std::suspend_never initial_suspend() noexcept
    {
        return {};
    }

    std::suspend_always final_suspend() noexcept
    {
        return {};
    }

    void return_void() noexcept {}

    void unhandled_exception()
    {
        throw;
    }

};

template<typename T>
class SyncWaitTask final
{
public:
    using promise_type = SyncWaitTaskPromise<T>;
private:
    using CoroHandle = std::coroutine_handle<promise_type>;
    CoroHandle handle;
public:
    
    SyncWaitTask(CoroHandle coroutine) noexcept
        : handle(coroutine)
    {}

    SyncWaitTask(SyncWaitTask&& other) noexcept
        : handle(std::exchange(other.handle, CoroHandle {}))
    {}

    ~SyncWaitTask()
    {
        if (handle) handle.destroy();
    }

    SyncWaitTask(const SyncWaitTask&) = delete;
    SyncWaitTask& operator=(const SyncWaitTask&) = delete;

    decltype(auto) result()
    {
        return handle.promise().result();
    }
};


template<typename T>
SyncWaitTask<T> startTask(Task<T>&& t)
{
    co_yield co_await std::move(t);
}

template<>
SyncWaitTask<void> startTask(Task<void>&& t)
{
    co_await std::move(t);
}

template<typename T>
std::future<SyncWaitTask<T>> startTaskAsync(Task<T>&& t)
{
    return std::async(std::launch::async, [&t]() {
        return startTask(std::move(t));
    });
}
