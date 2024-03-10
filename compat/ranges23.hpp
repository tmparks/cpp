#pragma once

// C++23 is newer than C++20
#if __cplusplus > 202002L

#include <ranges>

#include <ostream>
#include <tuple>

// See https://en.cppreference.com/w/cpp/utility/apply#Example
template <typename... Ts>
std::ostream& operator<<(std::ostream& stream, std::tuple<Ts...> const& tuple) {
    auto f = [&stream](Ts const&... args) {
        stream << '[';
        std::size_t n { 0 };
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

    namespace ranges {

        namespace views {

            // See [Function Aliases In C++](https://www.fluentcpp.com/2017/10/27/function-aliases-cpp/)
            template <typename... Args>
            inline auto zip(Args&&... args)
                    -> decltype(boost::combine(std::forward<Args>(args)...)) {
                return boost::combine(std::forward<Args>(args)...);
            }

        } // namespace views

    } // namespace ranges

} // namespace std

#endif // C++23
