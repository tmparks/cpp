#pragma once
#include "compat/gsl14.hpp"
#include <memory>

// Forward declarations.
template <typename T>
class UniqueRef;

template <typename T>
void swap(UniqueRef<T>& left, UniqueRef<T>& right) noexcept;

template <typename T, typename... Args>
UniqueRef<T> makeUniqueRef(Args&&... args);

// Inspired by std::reference_wrapper, but holds a unique_ptr
// instead of a raw pointer.
// Cannot be copied because unique_ptr cannot be copied.
// A moved-from object is empty, but can be deleted or assigned to.
// See: [A move operation should move and leave its source in a valid state](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rc-move-semantic)
template <typename T>
class UniqueRef {
public:
    using Type = T;
    virtual ~UniqueRef() noexcept = default; // destructor
    UniqueRef(std::unique_ptr<T> p) noexcept;
    void reset(std::unique_ptr<T> p) noexcept;
    operator T&();
    operator const T&() const;
    T& get();
    [[nodiscard]] const T& get() const;
    UniqueRef(UniqueRef&&) noexcept = default;            // move constructor
    UniqueRef& operator=(UniqueRef&&) noexcept = default; // move assignment
    friend void swap<>(UniqueRef&, UniqueRef&) noexcept;  // non-member swap

    // Unfortunately, we cannot overload operator . (dot)
    // See [Operator Dot](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4477.pdf)
    // T& operator.();
private:
    UniqueRef() noexcept = delete;                   // no default constructor
    UniqueRef(const UniqueRef&) = delete;            // no copy constructor
    UniqueRef& operator=(const UniqueRef&) = delete; // no copy assignment

    std::unique_ptr<T> p_; // never null (unless moved from)
};

template <typename T>
UniqueRef<T>::UniqueRef(std::unique_ptr<T> p) noexcept : p_{std::move(p)} {
    Expects(p_ != nullptr);
}

template <typename T>
void UniqueRef<T>::reset(std::unique_ptr<T> p) noexcept {
    Expects(p != nullptr);
    p_ = std::move(p);
}

template <typename T>
UniqueRef<T>::operator T&() {
    return *p_;
}

template <typename T>
UniqueRef<T>::operator const T&() const {
    return *p_;
}

template <typename T>
T& UniqueRef<T>::get() {
    return *p_;
}

template <typename T>
const T& UniqueRef<T>::get() const {
    return *p_;
}

// non-member swap
template <typename T>
void swap(UniqueRef<T>& left, UniqueRef<T>& right) noexcept {
    using std::swap; // enable Argument Dependent Lookup
    swap(left.p_, right.p_);
}

template <typename T, typename... Args>
UniqueRef<T> makeUniqueRef(Args&&... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}
