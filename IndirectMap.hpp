#pragma once

#include "IndirectContainer.hpp"

template <typename M>
class IndirectMap : public IndirectContainer<M> {
public:
    using size_type = typename IndirectContainer<M>::size_type;
    using iterator = typename IndirectContainer<M>::iterator;
    using const_iterator = typename IndirectContainer<M>::const_iterator;
    using IndirectContainer<M>::base;

    using key_type = typename M::key_type;
    using mapped_type =
            typename std::pointer_traits<typename M::mapped_type>::element_type;
    using value_type = typename std::pair<const key_type, mapped_type>;
    using key_compare = typename M::key_compare;
    using reference = value_type&;
    using const_reference = const value_type&;

    reference at(const key_type& key);
    const_reference at(const key_type& key) const;
    reference operator[](const key_type& key);
    reference operator[](key_type&& key);
    size_type count(const key_type& key) const;
    iterator find(const key_type& key);
    bool contains(const key_type& key) const;
    std::pair<iterator, iterator> equal_range(const key_type& key);
    iterator lower_bound(const key_type& key);
    const_iterator lower_bound(const key_type& key) const;
    iterator upper_bound(const key_type& key);
    const_iterator upper_bound(const key_type& key) const;
};

////////////////////////////////////////////////////////////////////////////////

template <typename M>
auto IndirectMap<M>::at(const key_type& key) -> reference {
    return *base.at(key);
}

template <typename M>
auto IndirectMap<M>::at(const key_type& key) const -> const_reference {
    return *base.at(key);
}

template <typename M>
auto IndirectMap<M>::operator[](const key_type& key) -> reference {
    return *base[key];
}

template <typename M>
auto IndirectMap<M>::operator[](key_type&& key) -> reference {
    return *base[std::move(key)];
}

template <typename M>
auto IndirectMap<M>::count(const key_type& key) const -> size_type {
    return base.count(key);
}

template <typename M>
auto IndirectMap<M>::find(const key_type& key) -> iterator {
    return base.find(key);
}

template <typename M>
auto IndirectMap<M>::contains(const key_type& key) const -> bool {
    return base.contains(key);
}

template <typename M>
auto IndirectMap<M>::equal_range(const key_type& key)
        -> std::pair<iterator, iterator> {
    auto&& result = base.equal_range(key);
    return {result.first, result.second};
}

template <typename M>
auto IndirectMap<M>::lower_bound(const key_type& key) -> iterator {
    return base.lower_bound(key);
}

template <typename M>
auto IndirectMap<M>::lower_bound(const key_type& key) const -> const_iterator {
    return base.lower_bound(key);
}

template <typename M>
auto IndirectMap<M>::upper_bound(const key_type& key) -> iterator {
    return base.upper_bound(key);
}
template <typename M>
auto IndirectMap<M>::upper_bound(const key_type& key) const -> const_iterator {
    return base.upper_bound(key);
}
