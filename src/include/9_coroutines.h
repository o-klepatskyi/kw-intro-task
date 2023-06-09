#include "Generator.h"
#include "Task.h"
#include "SyncWaitTask.h"

#include <coroutine>
#include <iostream>
#include <string>
#include <concepts>
#include <exception>

static const char* const EMPTY = ""; 

struct HelloNeverSuspend
{
    struct promise_type {
        std::suspend_never initial_suspend() noexcept { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }
        HelloNeverSuspend get_return_object() noexcept { return {}; }
        void unhandled_exception() { throw; }

        void return_void() noexcept {}
    };
};

static const char* const HELLO_FROM_CO_1 = "Hello from coroutine 1!\n";

HelloNeverSuspend helloNeverSuspend()
{
    std::cout << HELLO_FROM_CO_1;
    co_return;
}

// =======================================================================

struct HelloSuspend
{
    struct promise_type;

    using Handle = std::coroutine_handle<promise_type>;
    Handle handle;
    
    HelloSuspend(Handle h) : handle(h) {}

    ~HelloSuspend()
    {
        if (handle) handle.destroy();
    }

    void resume() const { handle.resume(); }

    struct promise_type {
        std::suspend_always initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        HelloSuspend get_return_object() noexcept
        {
            return HelloSuspend {
                std::coroutine_handle<promise_type>::from_promise(*this)
            };
        }
        void unhandled_exception() { throw; }

        void return_void() noexcept {}
    };
};

static const char* const HELLO_FROM_CO_2 = "Hello from coroutine 2!\n";

HelloSuspend helloSuspend()
{
    std::cout << HELLO_FROM_CO_2;
    co_return;
}

// =======================================================================

struct PrintMyString
{
    struct promise_type;

    using Handle = std::coroutine_handle<promise_type>;
    Handle handle;

    PrintMyString(Handle h) : handle(h) {}

    ~PrintMyString()
    {
        if (handle) handle.destroy();
    }

    void provide(std::string s)
    {
        if (not handle.done()) 
        {
            handle.promise().value = s;
            handle.resume();
        }
    }

    struct promise_type {
        std::string value;

        std::suspend_always initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        PrintMyString get_return_object() noexcept
        {
            return PrintMyString { Handle::from_promise(*this) };
        }

        void unhandled_exception() { throw; }

        void return_void() noexcept {}

        struct StringAwaitable
        {
            promise_type& pt;
            bool await_ready() { return true; }

            void await_suspend(Handle) {}
            
            std::string await_resume()
            {
                return pt.value;
            }
        };

        auto await_transform(std::string) noexcept
        {
            return StringAwaitable {*this};
        }
    };
};

PrintMyString printMyStringOnce()
{
    std::string str = co_await std::string {};
    std::cout << str;
    co_return;
}

// =======================================================================

Generator<std::string> readStream(std::istream& is)
{
    std::string input;
    while(true)
    {
        std::getline(is, input);
        co_yield input;
    }
}

// =======================================================================

template<std::integral T>
Generator<T> range(T start = 0, const T endInclusive = 0, const T step = 1) noexcept
{
    while (start <= endInclusive)
    {
        co_yield start;
        start += step;
    }
}

// =======================================================================

Task<> completes_synchronously() {
    co_return;
}

Task<> loop_synchronously(int count) {
    std::cout << "loop_synchronously(" << count << ")" << std::endl;
    for (int i = 0; i < count; ++i) {
        co_await completes_synchronously();
    }
    std::cout << "loop_synchronously(" << count << ") returning" << std::endl;
}

struct my_exception : public std::exception {};