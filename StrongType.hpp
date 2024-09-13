#pragma once
#include <cstdint>

// [Getting the Benefits of Strong Typing in C++ at a Fraction of the Cost](https://www.fluentcpp.com/2018/04/06/strong-types-by-struct/)

// A simple strong type.
template <typename T = intmax_t>
struct StrongType {
    using type = T; // underlying type
    T value{};      // zero-initialized value
};

// A simple strong identifier.
template <typename T = uintmax_t>
struct StrongIdentifier : StrongType<T> {
    const static StrongIdentifier<T> null;         // initialized later
    bool operator==(StrongIdentifier other) const; // equal
    bool operator<(StrongIdentifier other) const;  // less
};

template <typename T>
const StrongIdentifier<T> StrongIdentifier<T>::null{}; // zero-initialized

template <typename T>
bool StrongIdentifier<T>::operator==(StrongIdentifier other) const {
    return this->value == other.value;
}

template <typename T>
bool StrongIdentifier<T>::operator<(StrongIdentifier other) const {
    return this->value < other.value;
}
