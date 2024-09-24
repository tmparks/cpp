#pragma once

#if __cplusplus >= 202002L

#include <numbers>

#else // C++20

#pragma message "emulating C++20 numbers"

#include <cmath>

// NOLINTBEGIN(*-concat-nested-namespaces) until C++17

namespace std {
    namespace numbers {
        constexpr auto pi = M_PI;
    } // namespace numbers
} // namespace std

// NOLINTEND(*-concat-nested-namespaces)

#endif // C++20
