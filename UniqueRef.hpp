#pragma once

#include "Verbose.hpp"
#include "compat/gsl14.hpp"
#include <iostream>
#include <memory>

// Inspired by std::reference_wrapper, but holds a unique_ptr instead of a raw pointer.
// Cannot be copied because it maintains the same invariant as unique_ptr:
// that it is the sole owner of the managed object.
// Cannot be moved because it maintains the invariant that the contained
// unique_ptr is never null.
// These restrictions limit the ability to use UniqueRef with containers.
template <typename T>
class UniqueRef {
public:
    virtual ~UniqueRef() = default; // destructor
    UniqueRef(std::unique_ptr<T> p);
    void reset(std::unique_ptr<T> p);
    operator T&();
    operator const T&() const;
    T& get();
    const T& get() const;
    friend void swap<>(UniqueRef&, UniqueRef&) noexcept; // non-member swap

    // Unfortunately, we cannot overload operator . (dot)
    // See [Operator Dot](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4477.pdf)
    // T& operator.();
private:
    UniqueRef() = delete;                            // no default constructor
    UniqueRef(const UniqueRef&) = delete;            // no copy constructor
    UniqueRef(UniqueRef&&) noexcept = delete;        // no move constructor
    UniqueRef& operator=(const UniqueRef&) = delete; // no copy assignment
    UniqueRef& operator=(UniqueRef&&) noexcept = delete; // no move assignment

    std::unique_ptr<T> p_;
    Verbose v_ { "UniqueRef" };
};

template <typename T>
UniqueRef<T>::UniqueRef(std::unique_ptr<T> p) : p_ { std::move(p) } {
    std::cout << "conversion from unique_ptr" << std::endl;
    Ensures(p_ != nullptr);
}

template <typename T>
void UniqueRef<T>::reset(std::unique_ptr<T> p) {
    p_ = std::move(p);
    Ensures(p_ != nullptr);
}

template <typename T>
UniqueRef<T>::operator T&() {
    std::cout << "conversion to reference" << std::endl;
    return *p_;
}

template <typename T>
UniqueRef<T>::operator const T&() const {
    std::cout << "conversion to const reference" << std::endl;
    return *p_;
}

template <typename T>
T& UniqueRef<T>::get() {
    std::cout << gsl::czstring(__func__) << ": ";
    return *p_;
}

template <typename T>
const T& UniqueRef<T>::get() const {
    std::cout << gsl::czstring(__func__) << ": ";
    return *p_;
}

// non-member swap
template <typename T>
void swap(UniqueRef<T>& left, UniqueRef<T>& right) noexcept {
    using std::swap; // enable Argument Dependent Lookup
    swap(left.p_, right.p_);
    swap(left.v_, right.v_);
}
