#pragma once

#if __cplusplus >= 202002L

#include <numbers>

#else // C++20

#include <cmath>
namespace std {
    namespace numbers {
        constexpr double pi = M_PI;
    } // namespace numbers
} // namespace std

#endif // C++20
