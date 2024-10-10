#pragma once

#include <boost/iterator/transform_iterator.hpp>

template <typename M>
class IndirectMap {
public:
    using key_type = typename M::key_type;
    using mapped_type =
            typename std::pointer_traits<typename M::mapped_type>::element_type;
    using value_type = typename std::pair<const key_type&, mapped_type&>;
    using const_value_type =
            typename std::pair<const key_type&, const mapped_type&>;
    using size_type = typename M::size_type;
    using difference_type = typename M::difference_type;
    using key_compare = typename M::key_compare;
    using reference = value_type&;
    using const_reference = const_value_type&;
    using value_compare = M::value_compare;
    class value_transform;
    using iterator =
            boost::transform_iterator<value_transform, typename M::iterator>;
    using const_iterator =
            boost::transform_iterator<value_transform, typename M::const_iterator>;
    using reverse_iterator =
            boost::transform_iterator<value_transform, typename M::reverse_iterator>;
    using const_reverse_iterator =
            boost::transform_iterator<value_transform, typename M::const_reverse_iterator>;

    M base{};
    bool operator==(const IndirectMap& other) const noexcept;
    bool operator<(const IndirectMap& other) const noexcept;

    reference at(const key_type& key);
    [[nodiscard]] const_reference at(const key_type& key) const;

    reference operator[](const key_type& key);
    reference operator[](key_type&& key);

    iterator begin();
    [[nodiscard]] const_iterator begin() const;
    [[nodiscard]] const_iterator cbegin() const noexcept;

    iterator end() noexcept;
    [[nodiscard]] const_iterator end() const noexcept;
    [[nodiscard]] const_iterator cend() const noexcept;

    reverse_iterator rbegin();
    [[nodiscard]] const_reverse_iterator rbegin() const;
    [[nodiscard]] const_reverse_iterator crbegin() const noexcept;

    reverse_iterator rend();
    [[nodiscard]] const_reverse_iterator rend() const;
    [[nodiscard]] const_reverse_iterator crend() const noexcept;

    [[nodiscard]] size_type count(const key_type& key) const;
    iterator find(const key_type& key);
    [[nodiscard]] bool contains(const key_type& key) const;
    std::pair<iterator, iterator> equal_range(const key_type& key);
    iterator lower_bound(const key_type& key);
    [[nodiscard]] const_iterator lower_bound(const key_type& key) const;
    iterator upper_bound(const key_type& key);
    [[nodiscard]] const_iterator upper_bound(const key_type& key) const;
};

////////////////////////////////////////////////////////////////////////////////

template <typename M>
class IndirectMap<M>::value_transform {
public:
    IndirectMap<M>::value_type operator()(M::value_type& v) const {
        return {v.first, *v.second};
    }

    IndirectMap<M>::const_value_type operator()(const M::value_type& v) const {
        return {v.first, *v.second};
    }
};

////////////////////////////////////////////////////////////////////////////////

template <typename M>
auto IndirectMap<M>::operator==(const IndirectMap& other) const noexcept -> bool {
    return base == other.base;
}

template <typename M>
auto IndirectMap<M>::operator<(const IndirectMap& other) const noexcept -> bool {
    return base < other.base;
}

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
auto IndirectMap<M>::begin() -> iterator {
    return {base.begin(), value_transform{}};
}

template <typename M>
auto IndirectMap<M>::begin() const -> const_iterator {
    return {base.begin(), value_transform{}};
}

template <typename M>
auto IndirectMap<M>::cbegin() const noexcept -> const_iterator {
    return {base.cbegin(), value_transform{}};
}

template <typename M>
auto IndirectMap<M>::end() noexcept -> iterator {
    return {base.end(), value_transform{}};
}

template <typename M>
auto IndirectMap<M>::end() const noexcept -> const_iterator {
    return {base.end(), value_transform{}};
}

template <typename M>
auto IndirectMap<M>::cend() const noexcept -> const_iterator {
    return {base.cend(), value_transform{}};
}

template <typename M>
auto IndirectMap<M>::rbegin() -> reverse_iterator {
    return {base.rbegin(), value_transform{}};
}

template <typename M>
auto IndirectMap<M>::rbegin() const -> const_reverse_iterator {
    return {base.rbegin(), value_transform{}};
}

template <typename M>
auto IndirectMap<M>::crbegin() const noexcept -> const_reverse_iterator {
    return {base.crbegin(), value_transform{}};
}

template <typename M>
auto IndirectMap<M>::rend() -> reverse_iterator {
    return {base.rend(), value_transform{}};
}

template <typename M>
auto IndirectMap<M>::rend() const -> const_reverse_iterator {
    return {base.rend(), value_transform{}};
}

template <typename M>
auto IndirectMap<M>::crend() const noexcept -> const_reverse_iterator {
    return {base.crend(), value_transform{}};
}

template <typename M>
auto IndirectMap<M>::count(const key_type& key) const -> size_type {
    return base.count(key);
}

template <typename M>
auto IndirectMap<M>::find(const key_type& key) -> iterator {
    return {base.find(key), value_transform{}};
}

template <typename M>
auto IndirectMap<M>::contains(const key_type& key) const -> bool {
    return base.contains(key);
}

template <typename M>
auto IndirectMap<M>::equal_range(const key_type& key)
        -> std::pair<iterator, iterator> {
    auto&& result = base.equal_range(key);
    return {{result.first, value_transform{}},
            {result.second, value_transform{}}};
}

template <typename M>
auto IndirectMap<M>::lower_bound(const key_type& key) -> iterator {
    return {base.lower_bound(key), value_transform{}};
}

template <typename M>
auto IndirectMap<M>::lower_bound(const key_type& key) const -> const_iterator {
    return {base.lower_bound(key), value_transform{}};
}

template <typename M>
auto IndirectMap<M>::upper_bound(const key_type& key) -> iterator {
    return {base.upper_bound(key), value_transform{}};
}
template <typename M>
auto IndirectMap<M>::upper_bound(const key_type& key) const -> const_iterator {
    return {base.upper_bound(key), value_transform{}};
}
