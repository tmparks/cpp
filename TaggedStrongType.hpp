#pragma once

// [Getting the Benefits of Strong Typing in C++ at a Fraction of the Cost](https://www.fluentcpp.com/2018/04/06/strong-types-by-struct/)

// A strong type pattern that avoids inheritance so that [aggregate initialization](https://en.cppreference.com/w/cpp/language/aggregate_initialization)
// can be used, even before C++17.

// A simple strong type using type aliases.
// Example: using MyInt = StrongType<int, struct MyIntTag>;
template <typename T, typename UniqueTag>
struct StrongType {
    using type = T; // underlying type
    T value{};      // zero-initialized value
};

// A simple strong identifier using type aliases.
// Example: using MyId = StrongIdentifier<int, struct MyIdTag>;
template <typename T, typename UniqueTag>
struct StrongIdentifier : StrongType<T, UniqueTag> {
    const static StrongIdentifier null;            // initialized later
    bool operator==(StrongIdentifier other) const; // equal
    bool operator<(StrongIdentifier other) const;  // less
};

template <typename T, typename UniqueTag>
const StrongIdentifier<T, UniqueTag> StrongIdentifier<T, UniqueTag>::null{}; // zero-initialized

template <typename T, typename UniqueTag>
bool StrongIdentifier<T, UniqueTag>::operator==(StrongIdentifier other) const {
    return this->value == other.value;
}

template <typename T, typename UniqueTag>
bool StrongIdentifier<T, UniqueTag>::operator<(StrongIdentifier other) const {
    return this->value < other.value;
}
