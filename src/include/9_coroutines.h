#include <coroutine>
#include <iostream>
#include <string>

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

struct ReadStream
{
    struct promise_type;
    using Handle = std::coroutine_handle<promise_type>;

    Handle handle;

    struct promise_type
    {
        std::string output;

        ReadStream get_return_object() noexcept
        {
            return { Handle::from_promise(*this) };
        }

        std::suspend_always initial_suspend() noexcept { return {}; };
        void unhandled_exception() { throw; }
        std::suspend_never final_suspend() noexcept { return {}; }

        std::suspend_always yield_value(std::string s) noexcept
        {
            output = std::move(s);
            return {};
        }

        void return_void() noexcept {}
    };

    ReadStream(Handle h) noexcept : handle {h} {}

    ~ReadStream()
    {
        if (handle)
        {
            handle.destroy();
        }
    }

    std::string readLine() 
    {
        if (not handle.done())
        {
            handle.resume();
        }
        return std::move(handle.promise().output);
    }
};

ReadStream readStream(std::istream& is)
{
    std::string input;
    while(true)
    {
        std::getline(is, input);
        co_yield input;
    }
}