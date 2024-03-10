#pragma once

#if __cplusplus >= 201402L

#include <gsl/gsl>

#else // C++14

#pragma message "emulating C++14 gsl" // NOLINT

#include <cassert>
#define Expects assert
#define Ensures assert

namespace gsl {

    using czstring = std::string;
    using index = std::ptrdiff_t;
    template <typename T, typename U> T narrow_cast(U x) {
        return static_cast<T>(x);
    }

} // namespace gsl

#endif // C++14
