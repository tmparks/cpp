#pragma once
#include <cstdint>

// [Getting the Benefits of Strong Typing in C++ at a Fraction of the Cost](https://www.fluentcpp.com/2018/04/06/strong-types-by-struct/)

// A simple strong type.
template <typename T = intmax_t>
struct StrongType {
    using type = T; // underlying type
    T value{};      // zero-initialized value
#if __cplusplus < 201703L
    // Until C++17, [aggregate initialization](https://en.cppreference.com/w/cpp/language/aggregate_initialization)
    // cannot be used with derived types, so provide a constructor to inherit.
    using base = StrongType;                            // base type
    StrongType(T v) : value{v} {};                      // constructor
    StrongType() = default;                             // default constructor
    StrongType(StrongType&&) = default;                 // move constructor
    StrongType(const StrongType&) = default;            // copy constructor
    StrongType& operator=(StrongType&&) = default;      // move assignment
    StrongType& operator=(const StrongType&) = default; // copy assignment
    ~StrongType() = default;                            // non-virtual destructor
#endif // pre C++17
};

// A simple strong identifier.
template <typename T = uintmax_t>
struct StrongIdentifier : StrongType<T> {
    const static StrongIdentifier<T> null;         // initialized later
    bool operator==(StrongIdentifier other) const; // equal
    bool operator<(StrongIdentifier other) const;  // less
#if __cplusplus < 201703L
    using base = StrongIdentifier;   // base type
    using StrongType<T>::StrongType; // inherit constructors
#endif // pre C++17
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
