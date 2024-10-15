#pragma once
#include <iostream>
#include <string>

// Forward declarations
template <typename T>
class Verbose;

template <typename T>
void swap(Verbose<T>&, Verbose<T>&) noexcept;

// Make any class verbose
template <typename T>
class Verbose : public T {
public:
    template <typename... Args>
    Verbose(std::string name, Args&&... args);       // delegating constructor
    Verbose(const Verbose& other);                   // copy constructor
    Verbose(Verbose&& other) noexcept;               // move constructor
    Verbose& operator=(const Verbose& other);        // copy assignment
    Verbose& operator=(Verbose&& other) noexcept;    // move assignment
    ~Verbose() noexcept;                             // destructor NOLINT
    friend void swap<>(Verbose&, Verbose&) noexcept; // non-member swap

    std::string& name() noexcept;
    [[nodiscard]] const std::string& name() const noexcept;

private:
    std::string name_{};
};

template <typename T>
std::ostream& operator<<(std::ostream& stream, const Verbose<T>& object);

////////////////////////////////////////////////////////////////////////////////

template <typename T>
template <typename... Args>
Verbose<T>::Verbose(std::string name, Args&&... args) :
        T{std::forward<Args>(args)...}, name_(std::move(name)) {
    std::cout << name_ << ": constructor this=" << this << std::endl;
}

template <typename T>
Verbose<T>::Verbose(const Verbose& other) :
        T{other}, name_{std::string("copy constructed from ") + other.name_} {
    std::cout << name_ << ": copy constructor this=" << this
              << " other=" << &other << std::endl;
}

// Note that other.name_ is *not* moved from.
template <typename T>
Verbose<T>::Verbose(Verbose&& other) noexcept :
        T{std::move(other)},
        name_{std::string("move constructed from ") + other.name_} {
    std::cout << name_ << ": move constructor this=" << this
              << " other=" << &other << std::endl;
    other.name_ += " (moved)";
}

template <typename T>
Verbose<T>& Verbose<T>::operator=(const Verbose& other) {
    T::operator=(other);
    std::cout << name_ << ": copy assignment this=" << this
              << " other=" << &other << std::endl;
    name_ = std::string("copy assigned from ") + other.name_;
    return *this;
}

// Note that other.name_ is *not* moved from.
template <typename T>
Verbose<T>& Verbose<T>::operator=(Verbose&& other) noexcept {
    T::operator=(std::move(other));
    std::cout << name_ << ": move assignment this=" << this
              << " other=" << &other << std::endl;
    name_ = std::string("move assigned from ") + other.name_;
    other.name_ += " (moved)";
    return *this;
}

template <typename T>
Verbose<T>::~Verbose() noexcept {
    std::cout << name_ << ": destructor this=" << this << std::endl;
}

template <typename T>
void swap(Verbose<T>& left, Verbose<T>& right) noexcept {
    std::cout << left.name() << ": swap left=" << &left //
              << " right=" << &right << std::endl;
    using std::swap; // enable Argument Dependent Lookup
    swap(static_cast<T&>(left), static_cast<T&>(right));
    swap(left.name_, right.name_);
}

template <typename T>
std::string& Verbose<T>::name() noexcept {
    return name_;
}

template <typename T>
const std::string& Verbose<T>::name() const noexcept {
    return name_;
}

template <typename T>
std::ostream& operator<<(std::ostream& stream, const Verbose<T>& object) {
    return operator<<(stream, object.name());
}
