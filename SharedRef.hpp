#pragma once

#include "Verbose.hpp"
#include <iostream>
#include <memory>

// Inspired by std::reference_wrapper,
// but holds a shared_ptr instead of a raw pointer.
template <typename T>
class SharedRef {
public:
    template <typename... Args>
    SharedRef(Args&&... args);
    SharedRef(std::shared_ptr<T> p); // by value
    operator T&();
    operator const T&() const;
    operator std::shared_ptr<T>();
    operator std::shared_ptr<const T>() const;

    T& getRef();
    const T& getRef() const;
    std::shared_ptr<T> getPtr();
    std::shared_ptr<const T> getPtr() const;

    // Unfortunately, we cannot overload operator .
    // See [Operator Dot](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4477.pdf)
    // T& operator.();
private:
    std::shared_ptr<T> p_;
    Verbose v_ { "SharedRef" };
};

template <typename T>
template <typename... Args>
SharedRef<T>::SharedRef(Args&&... args) :
        p_ { std::make_shared<T>(std::forward<Args>(args)...) } { }

template <typename T>
SharedRef<T>::SharedRef(std::shared_ptr<T> p) : p_ { std::move(p) } {
    std::cout << "conversion from shared_ptr" << std::endl;
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
    std::cout << __func__ << ": ";
    return *this;
}
template <typename T>
const T& SharedRef<T>::getRef() const {
    std::cout << __func__ << ": ";
    return *this;
}
template <typename T>
std::shared_ptr<T> SharedRef<T>::getPtr() {
    std::cout << __func__ << ": ";
    return *this;
}
template <typename T>
std::shared_ptr<const T> SharedRef<T>::getPtr() const {
    std::cout << __func__ << ": ";
    return *this;
}
