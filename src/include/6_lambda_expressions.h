#pragma once
#include <concepts>

template<typename... Args, std::invocable<Args...> Func>
constexpr decltype(auto) caller(Func f, Args&&... args) noexcept(std::is_nothrow_invocable<Func, Args...>)
{
    return f(std::forward<Args>(args)...);
};

struct Context
{
    std::unique_ptr<int> context_value = std::make_unique<int>(42);

    inline void changeContext(int val)
    {
        context_value = std::make_unique<int>(val);
    }

    inline auto getLambdaWithCapturedContext() const noexcept
    {
        // do not bound labmda to object, as object lifetime can be
        // less than lambda lifetime
        return [context = *context_value]() {
            return context;
        };
    }
};

// error-prone for overflows
// template is needed to be sure types are equal
auto avgL = []<typename T>(const T& lhs, const T& rhs) { return (lhs + rhs) / 2; };