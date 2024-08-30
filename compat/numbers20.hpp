#pragma once

#if __cplusplus >= 202002L

#include <numbers>

#else // C++20

#pragma message "emulating C++20 numbers"

#include <cmath>
namespace std {
    namespace numbers {
        constexpr auto pi = M_PI;
    } // namespace numbers
} // namespace std

#endif // C++20
