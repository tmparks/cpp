#pragma once
#include <cstdint>

// [Getting the Benefits of Strong Typing in C++ at a Fraction of the Cost](https://www.fluentcpp.com/2018/04/06/strong-types-by-struct/)

// A simple strong type.
template <typename T = intmax_t>
struct StrongType {
    using type = T; // underlying type
    T value{};      // default-initialized value
};

// A simple strong identifier.
template <typename T = uintmax_t>
struct StrongIdentifier : StrongType<T> {
    constexpr static T null = 0;
};
