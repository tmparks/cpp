#pragma once

// [Getting the Benefits of Strong Typing in C++ at a Fraction of the Cost](https://www.fluentcpp.com/2018/04/06/strong-types-by-struct/)

// A simple strong type using inheritance.
// Example until C++17: struct MyInt : StrongType<int> { using base::base; };
// Example since C++17: struct MyInt : StrongType<int> { };
template <typename T>
struct StrongType {
    using type = T; // underlying type
    T value{};      // zero-initialized value
#if __cplusplus < 201703L
    // Provide an explicit constructor that can be inherited because
    // [aggregate initialization](https://en.cppreference.com/w/cpp/language/aggregate_initialization)
    // cannot be used with derived types until C++17.
    using base = StrongType;                     // base type
    explicit StrongType(T v) : value{v} {};      // constructor
    StrongType() noexcept = default;             // default constructor
    StrongType(const StrongType&) = default;     // copy constructor
    StrongType(StrongType&&) noexcept = default; // move constructor
    StrongType& operator=(const StrongType&) = default;     // copy assignment
    StrongType& operator=(StrongType&&) noexcept = default; // move assignment
    ~StrongType() noexcept = default; // CAUTION: non-virtual destructor
#endif // until C++17
};

// A simple strong identifier using inheritance.
// Example until C++17: struct MyId : StrongIdentifier<int> { using base::base; };
// Example since C++17: struct MyId : StrongIdentifier<int> { };
template <typename T>
struct StrongIdentifier : StrongType<T> {
    const static StrongIdentifier null;         // initialized later
    bool operator==(StrongIdentifier other) const noexcept; // equal
    bool operator<(StrongIdentifier other) const noexcept;  // less
#if __cplusplus < 201703L
    using base = StrongIdentifier;   // base type
    using StrongType<T>::StrongType; // inherit constructors
#endif // until C++17
};

template <typename T>
const StrongIdentifier<T> StrongIdentifier<T>::null{}; // zero-initialized

template <typename T>
bool StrongIdentifier<T>::operator==(StrongIdentifier other) const noexcept {
    return this->value == other.value;
}

template <typename T>
bool StrongIdentifier<T>::operator<(StrongIdentifier other) const noexcept {
    return this->value < other.value;
}
