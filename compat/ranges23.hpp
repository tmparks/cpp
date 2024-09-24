#pragma once

// Note: GCC does not (yet) use 202302L for C++23,
// so check for anything strictly newer than C++20
// See [Predefined macros](https://en.cppreference.com/w/cpp/preprocessor/replace#Predefined_macros)
#if __cplusplus > 202002L

#include <ranges>

#include <ostream>
#include <tuple>

// See https://en.cppreference.com/w/cpp/utility/apply#Example
template <typename... Ts>
std::ostream& operator<<(std::ostream& stream, std::tuple<Ts...> const& tuple) {
    auto f = [&stream](Ts const&... args) {
        stream << '[';
        auto n = std::size_t { 0 };
        ((stream << args << (++n < sizeof...(Ts) ? ", " : "")), ...);
        stream << ']';
    };
    std::apply(f, tuple);
    return stream;
}

#else // C++23

#pragma message "emulating C++23 ranges"

#include <boost/range/combine.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_io.hpp>

namespace std {

    using boost::get;

    namespace ranges::views {

        // See [Function Aliases In C++](https://www.fluentcpp.com/2017/10/27/function-aliases-cpp/)
        template <typename... Args>
        inline auto zip(Args&&... args)
                -> decltype(boost::combine(std::forward<Args>(args)...)) {
            return boost::combine(std::forward<Args>(args)...);
        }

    } // namespace ranges::views

} // namespace std

#endif // C++23
