#pragma once

#include "Verbose.hpp"
#include "compat/gsl14.hpp"
#include <iostream>
#include <memory>

// Inspired by std::reference_wrapper, but holds a shared_ptr instead of a raw pointer.
// Cannot be moved because it maintains the invariant that the contained
// shared_ptr is never null.
template <typename T>
class SharedRef {
public:
    virtual ~SharedRef() = default; // destructor
    SharedRef(std::shared_ptr<T> p);
    void reset(std::shared_ptr<T> p);
    operator T&();
    operator const T&() const;
    operator std::shared_ptr<T>();
    operator std::shared_ptr<const T>() const;
    T& getRef();
    const T& getRef() const;
    std::shared_ptr<T> getPtr();
    std::shared_ptr<const T> getPtr() const;
    SharedRef(const SharedRef&) = default;               // copy constructor
    SharedRef& operator=(const SharedRef&) = default;    // copy assignment
    friend void swap<>(SharedRef&, SharedRef&) noexcept; // non-member swap

    // Unfortunately, we cannot overload operator . (dot)
    // See [Operator Dot](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4477.pdf)
    // T& operator.();
private:
    SharedRef() = delete;                     // no default constructor
    SharedRef(SharedRef&&) noexcept = delete; // no move constructor
    SharedRef& operator=(SharedRef&&) noexcept = delete; // no move assignment

    std::shared_ptr<T> p_;
    Verbose v_ { "SharedRef" };
};

template <typename T>
SharedRef<T>::SharedRef(std::shared_ptr<T> p) : p_ { std::move(p) } {
    std::cout << "conversion from shared_ptr" << std::endl;
    Ensures(p_ != nullptr);
}

template <typename T>
void SharedRef<T>::reset(std::shared_ptr<T> p) {
    p_ = std::move(p);
    Ensures(p_ != nullptr);
}

template <typename T>
SharedRef<T>::operator T&() {
    std::cout << "conversion to reference" << std::endl;
    return *p_;
}

template <typename T>
SharedRef<T>::operator const T&() const {
    std::cout << "conversion to const reference" << std::endl;
    return *p_;
}

template <typename T>
SharedRef<T>::operator std::shared_ptr<T>() {
    std::cout << "conversion to shared_ptr" << std::endl;
    return p_;
}

template <typename T>
SharedRef<T>::operator std::shared_ptr<const T>() const {
    std::cout << "conversion to const shared_ptr" << std::endl;
    return p_;
}

template <typename T>
T& SharedRef<T>::getRef() {
    std::cout << gsl::czstring(__func__) << ": ";
    return *p_;
}

template <typename T>
const T& SharedRef<T>::getRef() const {
    std::cout << gsl::czstring(__func__) << ": ";
    return *p_;
}

template <typename T>
std::shared_ptr<T> SharedRef<T>::getPtr() {
    std::cout << gsl::czstring(__func__) << ": ";
    return p_;
}

template <typename T>
std::shared_ptr<const T> SharedRef<T>::getPtr() const {
    std::cout << gsl::czstring(__func__) << ": ";
    return p_;
}

// non-member swap
template <typename T>
void swap(SharedRef<T>& left, SharedRef<T>& right) noexcept {
    using std::swap; // enable Argument Dependent Lookup
    swap(left.p_, right.p_);
    swap(left.v_, right.v_);
}
