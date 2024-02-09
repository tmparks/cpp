#pragma once

#if __cplusplus >= 202302L

#include <ranges>

#else // C++23

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
