#pragma once
#include <variant>
#include <concepts>
#include <type_traits>
struct Gadget {};

enum class Consumed : int
{
    LVALUE = 1,
    RVALUE,
    INT
};

template<typename T>
struct ReferenceConsumer
{
    static constexpr auto f(T&)  noexcept { return Consumed::LVALUE; }
    static constexpr auto f(T&&) noexcept { return Consumed::RVALUE; }
};

template<typename T>
using RefConsumer = ReferenceConsumer<std::remove_reference_t<T>>;

template<typename T>
constexpr inline auto consume(T&& ref) noexcept
{
    return RefConsumer<T>::f(std::forward<T>(ref));
};

template<typename T>
inline auto overrideForInts(T&& ref) noexcept
{
    return RefConsumer<T>::f(std::forward<T>(ref));
};

inline auto overrideForInts(int) noexcept
{
    return Consumed::INT;
};

// std::variant also uses the same approach with std::enable_if
// modernized for concepts
template<typename... Args>
struct AnyConsumer
{
    // nullptr is used for moving and as a default value
    using MyVariant = std::variant<std::nullptr_t, Args...>;
    MyVariant value;

    explicit AnyConsumer() : value {nullptr} {}

    template<typename T>
    explicit AnyConsumer(T&& ref)
    requires !std::derived_from<std::decay_t<T>, AnyConsumer>
    : value {std::forward<T>(ref)} {}

    explicit AnyConsumer(const AnyConsumer&)  = default;
    explicit AnyConsumer(AnyConsumer&& other)
    : value{ std::move(other.value) }
    {
        other.value = nullptr;
    }
};