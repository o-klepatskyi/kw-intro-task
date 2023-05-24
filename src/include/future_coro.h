#pragma once

#include <coroutine>
#include <future>
#include <exception> // std::current_exception
#include <memory>
#include <cassert>

/**
 * Enable the use of rpp::cfuture<T> as a coroutine type
 * by using a rpp::cpromise<T> as the promise type.
 *
 * The most flexible way to do this is to define a specialization
 * of std::coroutine_traits<>
 *
 * This does not provide any async behaviors - simply allows easier interop
 * between existing rpp::cfuture<T> async functions
 */
template <typename T, typename... Args>
    requires(!std::is_void_v<T> && !std::is_reference_v<T>)
struct std::coroutine_traits<std::future<T>, Args...>
{
    struct promise_type : std::promise<T>
    {
        std::future<T> get_return_object() noexcept
        {
            return std::future{ this->get_future() };
        }

        std::suspend_never initial_suspend() const noexcept { return {}; }
        std::suspend_never final_suspend() const noexcept { return {}; }

        void return_value(const T& value) noexcept(std::is_nothrow_copy_constructible_v<T>)
        {
            this->set_value(value);
        }

        void return_value(T&& value) noexcept(std::is_nothrow_move_constructible_v<T>)
        {
            this->set_value(std::move(value));
        }

        void unhandled_exception() noexcept
        {
            this->set_exception(std::current_exception());
        }
    };
};

template <typename... Args>
struct std::coroutine_traits<std::future<void>, Args...>
{
    struct promise_type : std::promise<void>
    {
        std::future<void> get_return_object() noexcept
        {
            return std::future { this->get_future() };
        }

        std::suspend_never initial_suspend() const noexcept { return {}; }
        std::suspend_never final_suspend() const noexcept { return {}; }

        void return_void() noexcept
        {
            this->set_value();
        }

        void unhandled_exception() noexcept
        {
            this->set_exception(std::current_exception());
        }
    };
};

template<typename T>
struct awaiter : std::future<T>
{
    // check if this future is ready yet?
    bool await_ready() const noexcept
    {
        using namespace std::chrono_literals;
        return this->wait_for(0s) != std::future_status::timeout;
    }

    // suspension point that launches the background async task
    void await_suspend(std::coroutine_handle<> cont) noexcept
    {
        std::future<T> me = std::move(*this);
        std::promise<T> p;
        std::future<T>::operator=(p.get_future());
        std::thread t(
            [cont] (std::future<T>&& f, std::promise<T>&& p) { // TODO is it safe to move?
                try
                {
                    auto result = f.get();
                    cont.resume();
                    p.set_value(result);
                }
                catch(...)
                {
                    p.set_exception(std::current_exception());
                }
            }, std::move(me), std::move(p)
        );
        t.detach();

        // due to rpp::cfuture<T>::continue_with implementation
        // `f` will be moved into the continuation, so we need a copy
        // f.continue_with([this, cont] (const T&)
        // {
        //     cont.resume(); // signal ready!
        // });
    }

    // resume & get the value once `cont()` is signaled
    T await_resume()
    {
        return this->get();
    }
};

template<>
struct awaiter<void> : public std::future<void>
{
private:
    // awaiter<void>& operator=(std::future<void>&& rhs) noexcept
    // {
        
    //     return *this;
    // }
public:

    // check if this future is ready yet?
    bool await_ready() const noexcept
    {
        using namespace std::chrono_literals;
        return this->wait_for(0s) != std::future_status::timeout;
    }

    // suspension point that launches the background async task
    void await_suspend(std::coroutine_handle<> cont) noexcept
    {
        std::future<void> me = std::move(*this);
        std::promise<void> p;
        std::future<void>::operator=(p.get_future());
        std::thread t(
            [cont] (std::future<void>&& f, std::promise<void>&& p) { // TODO is it safe to move?
                try
                {
                    f.get();
                    cont.resume();
                    p.set_value();
                }
                catch(...)
                {
                    p.set_exception(std::current_exception());
                }
            }, std::move(me), std::move(p)
        );
        t.detach();

        // due to rpp::cfuture<T>::continue_with implementation
        // `f` will be moved into the continuation, so we need a copy
        // awaiter f = *this; 
        // f.continue_with([this, cont]
        // {
        //     cont(); // signal ready!
        // });
    }

    // resume & get the value once `cont()` is signaled
    void await_resume()
    {
        this->get();
    }
};


/**
 * Allow co_await'ing rpp::cfuture<T> and rpp::cfuture<void> by calling 
 * rpp::cfuture<T>::continue_with() async continuation for each co_await.
 */
template<typename T>
auto operator co_await(std::future<T> future) noexcept
{
    return awaiter<T> { std::move(future) };
}

/**
 * @brief Allows awaiting on a lambda
 * TODO: Add lambda arguments
 */
template<class Task>
struct lambda_awaiter
{
    Task action;
    using T = decltype(action());
    std::unique_ptr<std::future<T>> result;

    explicit lambda_awaiter(Task&& task) noexcept : action{ std::move(task) } {}

    // is the task ready?
    bool await_ready() const noexcept
    {
        if (!result) // task hasn't even been created yet!
        {
            return false;
        }
        using namespace std::chrono_literals;
        return result->wait_for(0s) != std::future_status::timeout;
    }

    // suspension point that launches the background async task
    void await_suspend(std::coroutine_handle<> cont) noexcept
    {
        if (result) std::terminate(); // avoid task explosion
        std::promise<T> p;
        result = std::make_unique<std::future<T>>(p.get_future());

        std::thread t([this, cont] (std::promise<T>&& promise)
        {
            try
            {
                promise.set_value(action());
            }
            catch(...)
            {
                promise.set_exception(std::current_exception());
            }
            cont.resume();
        }, std::move(p));
        t.detach();
    }

    // resume & get the value once `cont()` is signaled
    T await_resume() noexcept
    {
        assert(result);
        return result->get(); // TODO: fix bug with exceptions
    }
};

/**
 * @brief Allow co_await'ing any invocable functors via rpp::async_task
 */
template<typename Task>
    requires std::is_invocable_v<Task>
lambda_awaiter<Task> operator co_await(Task&& task) noexcept
{
    return lambda_awaiter<Task>{ std::move(task) };
}