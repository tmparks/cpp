#pragma once

#include "IndirectContainer.hpp"

template <typename S>
class IndirectSequence : public IndirectContainer<S> {
public:
    using size_type = typename IndirectContainer<S>::size_type;
    using IndirectContainer<S>::base;

    using value_type =
            typename std::pointer_traits<typename S::value_type>::element_type;
    using reference = value_type&;
    using const_reference = const value_type&;

    reference at(size_type pos);
    const_reference at(size_type pos) const;
    reference operator[](size_type pos);
    const_reference operator[](size_type pos) const;
    reference front();
    const_reference front() const;
    reference back();
    const_reference back() const;
};

////////////////////////////////////////////////////////////////////////////////

template <typename S>
auto IndirectSequence<S>::at(size_type pos) -> reference {
    return *base.at(pos);
}

template <typename S>
auto IndirectSequence<S>::at(size_type pos) const -> const_reference {
    return *base.at(pos);
}

template <typename S>
auto IndirectSequence<S>::operator[](size_type pos) -> reference {
    return *base[pos];
}

template <typename S>
auto IndirectSequence<S>::operator[](size_type pos) const -> const_reference {
    return *base[pos];
}

template <typename S>
auto IndirectSequence<S>::front() -> reference {
    return *base.front();
}

template <typename S>
auto IndirectSequence<S>::front() const -> const_reference {
    return *base.front();
}

template <typename S>
auto IndirectSequence<S>::back() -> reference {
    return *base.back();
}

template <typename S>
auto IndirectSequence<S>::back() const -> const_reference {
    return *base.back();
}
