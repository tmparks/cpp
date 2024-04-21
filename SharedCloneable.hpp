#pragma once
#include "SharedObject.hpp"
#include "compat/gsl14.hpp"

// [The ‘clonable’ pattern](https://herbsutter.com/2019/10/03/gotw-ish-solution-the-clonable-pattern/)
// [Polymorphic clones in modern C++](https://www.fluentcpp.com/2017/09/08/make-polymorphic-copy-modern-cpp/)
// [How to Return a Smart Pointer AND Use Covariance](https://www.fluentcpp.com/2017/09/12/how-to-return-a-smart-pointer-and-use-covariance/)

template <typename Derived, typename Base = SharedObject>
class SharedCloneable : public Base {
public:
    std::shared_ptr<Derived> clone() const;
    ~SharedCloneable() override = default;

protected:
    using Base::Base; // Inherit all constructors from Base.
    SharedCloneable() = default;
    SharedCloneable(SharedCloneable&&) = delete;
    SharedCloneable(const SharedCloneable&) = default;
    SharedCloneable& operator=(SharedCloneable&&) = delete;
    SharedCloneable& operator=(const SharedCloneable&) = delete;

private:
    virtual std::shared_ptr<SharedObject> cloneImpl() const; // NOLINT *-explicit-virtual-functions
};

template <typename Derived, typename Base>
std::shared_ptr<Derived> SharedCloneable<Derived, Base>::clone() const {
    auto result = std::static_pointer_cast<Derived>(std::move(cloneImpl()));

    // Derived class must override cloneImpl!
    Ensures(typeid(*result) == typeid(*this));
    static_assert(
            not (std::is_copy_constructible_v<Derived>
                 || std::is_move_constructible_v<Derived>
                 || std::is_copy_assignable_v<Derived>
                 || std::is_move_assignable_v<Derived>),
            "Derived class must not be copy/move constructible/assignable!");

    return result;
}

template <typename Derived, typename Base>
std::shared_ptr<SharedObject> SharedCloneable<Derived, Base>::cloneImpl() const {
    return create<Derived>(static_cast<const Derived&>(*this));
}
