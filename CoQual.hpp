#pragma once
#include <utility>

// Apply the const qualification of the pointer to the referenced object.
// Thus, a const pointer is also a pointer to const.
template <typename P>
class CoQual : public P {
public:
    using element_type = typename P::element_type;
    using P::P; // inherit constructors

    CoQual() noexcept = default;                    // default constructor
    CoQual(const CoQual&) = default;                // copy constructor
    CoQual(CoQual&&) noexcept = default;            // move constructor
    CoQual& operator=(const CoQual&) = default;     // copy assignment
    CoQual& operator=(CoQual&&) noexcept = default; // move assignment
    ~CoQual() noexcept = default;                   // destructor

    CoQual(const P&);                // copy from base
    CoQual(P&&) noexcept;            // move from base
    CoQual& operator=(const P&);     // copy from base
    CoQual& operator=(P&&) noexcept; // move from base

    element_type* get() noexcept;
    [[nodiscard]] const element_type* get() const noexcept;
    element_type& operator*() noexcept;
    const element_type& operator*() const noexcept;
    element_type* operator->() noexcept;
    const element_type* operator->() const noexcept;
};

////////////////////////////////////////////////////////////////////////////////

template <typename P>
CoQual<P>::CoQual(const P& p) : P{p} {};

template <typename P>
CoQual<P>::CoQual(P&& p) noexcept : // NOLINT(*-not-moved)
        P{std::forward<P>(p)} {};

template <typename P>
auto CoQual<P>::operator=(const P& p) -> CoQual& {
    return P::operator=(p); // NOLINT(*-copy-assignment-signature)
}

template <typename P>
auto CoQual<P>::operator=(P&& p) noexcept -> CoQual& { // NOLINT(*-not-moved)
    return P::operator=(std::forward<P>(p)); // NOLINT(*-copy-assignment-signature)
}

template <typename P>
auto CoQual<P>::get() noexcept -> element_type* {
    return P::get();
}

template <typename P>
auto CoQual<P>::get() const noexcept -> const element_type* {
    return P::get();
}

template <typename P>
auto CoQual<P>::operator*() noexcept -> element_type& {
    return P::operator*();
}

template <typename P>
auto CoQual<P>::operator*() const noexcept -> const element_type& {
    return P::operator*();
}

template <typename P>
auto CoQual<P>::operator->() noexcept -> element_type* {
    return P::operator->();
}

template <typename P>
auto CoQual<P>::operator->() const noexcept -> const element_type* {
    return P::operator->();
}
