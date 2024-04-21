#pragma once
#include <memory>
#include <type_traits>

// Derived publicly from [std::enable_shared_from_this](https://en.cppreference.com/w/cpp/memory/enable_shared_from_this#Example)
class SharedObject : public std::enable_shared_from_this<SharedObject> {
public:
    // Non-member factory function. Any number of arguments.
    template <typename Derived, typename... Args>
    friend std::shared_ptr<Derived> create(Args&&... args);

    virtual ~SharedObject() = default;

protected:
    struct Protected; // Tag type available to derived classes.

    SharedObject() = default;
    SharedObject(SharedObject&& other) = default;
    SharedObject(const SharedObject& other) = default;
    SharedObject& operator=(SharedObject&& other) = default;
    SharedObject& operator=(const SharedObject& other) = default;
};

// [Making sure that people use make_unique and make_shared to make your object](https://devblogs.microsoft.com/oldnewthing/20220721-00/?p=106879)
struct SharedObject::Protected {
    explicit Protected() = default;
};

template <typename Derived, typename... Args>
std::shared_ptr<Derived> create(Args&&... args) {
    static_assert(
            std::is_convertible<Derived*, SharedObject*>::value,
            "SharedObject must be a public base class of Derived!");
    static_assert(
            not (std::is_default_constructible<Derived>::value
                 || std::is_copy_constructible<Derived>::value
                 || std::is_move_constructible<Derived>::value),
            "Constructors should not be publicly accessible!");
    return std::make_shared<Derived>(
            SharedObject::Protected {}, std::forward<Args>(args)...);
}
