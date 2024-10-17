#pragma once

#include <memory>

#if __cplusplus < 201402L // until C++14

#pragma message "emulating C++14 memory"

namespace std {

    template <typename T, typename... Args>
    typename std::enable_if<! std::is_array<T>::value, std::unique_ptr<T>>::type
            make_unique(Args&&... args) {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }

    template <typename T>
    typename std::enable_if<std::is_array<T>::value, std::unique_ptr<T>>::type
            make_unique(std::size_t n) {
        return std::unique_ptr<T>(new typename std::remove_extent<T>::type[n]);
    }

} // namespace std

#endif // C++14