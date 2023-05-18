#pragma once

template<typename T, typename U, bool b>
struct SelectT
{
    using value = T;
};

template<typename T, typename U>
struct SelectT<T, U, false>
{
    using value = U;
};

template<typename T, typename U, bool b>
using Select = SelectT<T, U, b>::value;