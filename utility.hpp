#pragma once
#include <type_traits>

// See [std::as_const](https://en.cppreference.com/w/cpp/utility/as_const)
template <typename T>
std::remove_const_t<T>& as_mutable(T& t) noexcept
{
    return const_cast<std::remove_const_t<T>&>(t);
}
