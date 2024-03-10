#pragma once

#if __cplusplus >= 202002L

#include <numbers>

#else // C++20

#pragma message "emulating C++20 numbers"  // NOLINT

#include <cmath>
namespace std {
    namespace numbers {
        constexpr double pi = M_PI;
    } // namespace numbers
} // namespace std

#endif // C++20
