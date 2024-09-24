#pragma once

#include "Verbose.hpp"
#include "compat/gsl14.hpp"
#include <iostream>
#include <memory>

// Forward declarations.
template <typename T>
class SharedRef;

template <typename T>
void swap(SharedRef<T>& left, SharedRef<T>& right) noexcept;

// Inspired by std::reference_wrapper, but holds a shared_ptr instead of a raw pointer.
// Cannot be moved because the held shared_ptr must never be null.
template <typename T>
class SharedRef {
public:
    using Type = T;
    virtual ~SharedRef() noexcept = default; // destructor
    SharedRef(std::shared_ptr<T> p) noexcept;
    void reset(std::shared_ptr<T> p) noexcept;
    operator T&();
    operator const T&() const;
    T& get();
    [[nodiscard]] const T& get() const;
    operator std::shared_ptr<T>();
    operator std::shared_ptr<const T>() const;
    std::shared_ptr<T> share();
    [[nodiscard]] std::shared_ptr<const T> share() const;
    SharedRef(const SharedRef&) = default;               // copy constructor
    SharedRef& operator=(const SharedRef&) = default;    // copy assignment
    friend void swap<>(SharedRef&, SharedRef&) noexcept; // non-member swap

    // Unfortunately, we cannot overload operator . (dot)
    // See [Operator Dot](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4477.pdf)
    // T& operator.();
private:
    SharedRef() noexcept = delete;            // no default constructor
    SharedRef(SharedRef&&) noexcept = delete; // no move constructor
    SharedRef& operator=(SharedRef&&) noexcept = delete; // no move assignment

    std::shared_ptr<T> p_; // never null
    Verbose v_ { "SharedRef" };
};

template <typename T>
SharedRef<T>::SharedRef(std::shared_ptr<T> p) noexcept : p_ { std::move(p) } {
    Expects(p_ != nullptr);
    std::cout << v_ << ": conversion from shared_ptr" << std::endl;
}

template <typename T>
void SharedRef<T>::reset(std::shared_ptr<T> p) noexcept {
    Expects(p != nullptr);
    p_ = std::move(p);
}

template <typename T>
SharedRef<T>::operator T&() {
    std::cout << v_ << ": conversion to reference" << std::endl;
    return *p_;
}

template <typename T>
SharedRef<T>::operator const T&() const {
    std::cout << v_ << ": conversion to const reference" << std::endl;
    return *p_;
}

template <typename T>
T& SharedRef<T>::get() {
    return *p_;
}

template <typename T>
const T& SharedRef<T>::get() const {
    return *p_;
}

template <typename T>
SharedRef<T>::operator std::shared_ptr<T>() {
    std::cout << v_ << ": conversion to shared_ptr" << std::endl;
    return p_;
}

template <typename T>
SharedRef<T>::operator std::shared_ptr<const T>() const {
    std::cout << v_ << ": conversion to const shared_ptr" << std::endl;
    return p_;
}

template <typename T>
std::shared_ptr<T> SharedRef<T>::share() {
    return p_;
}

template <typename T>
std::shared_ptr<const T> SharedRef<T>::share() const {
    return p_;
}

// non-member swap
template <typename T>
void swap(SharedRef<T>& left, SharedRef<T>& right) noexcept {
    using std::swap; // enable Argument Dependent Lookup
    swap(left.p_, right.p_);
    swap(left.v_, right.v_);
}
