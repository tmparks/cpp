#pragma once

#include <boost/iterator/indirect_iterator.hpp>

template <typename C>
class IndirectContainer {
public:
    using size_type = typename C::size_type;
    using difference_type = typename C::difference_type;
    using iterator = boost::indirect_iterator<typename C::iterator>;
    using const_iterator = boost::indirect_iterator<typename C::const_iterator>;
    using reverse_iterator =
            boost::indirect_iterator<typename C::reverse_iterator>;
    using const_reverse_iterator =
            boost::indirect_iterator<typename C::const_reverse_iterator>;

    // Base access

    C base{};

    // Comparison

    bool operator==(const IndirectContainer& other) const noexcept;
    bool operator<(const IndirectContainer& other) const noexcept;

    // Iterators

    iterator begin();
    const_iterator begin() const;
    const_iterator cbegin() const noexcept;

    iterator end() noexcept;
    const_iterator end() const noexcept;
    const_iterator cend() const noexcept;

    reverse_iterator rbegin();
    const_reverse_iterator rbegin() const;
    const_reverse_iterator crbegin() const noexcept;

    reverse_iterator rend();
    const_reverse_iterator rend() const;
    const_reverse_iterator crend() const noexcept;

    // Capacity

    [[nodiscard]] bool empty() const;
    size_type size() const;

    // Modifiers

    void clear();
};

////////////////////////////////////////////////////////////////////////////////

template <typename C>
auto IndirectContainer<C>::operator==(
        const IndirectContainer& other) const noexcept -> bool {
    return base == other.base;
}

template <typename C>
auto IndirectContainer<C>::operator<(
        const IndirectContainer& other) const noexcept -> bool {
    return base < other.base;
}

template <typename C>
auto IndirectContainer<C>::begin() -> iterator {
    return base.begin();
}

template <typename C>
auto IndirectContainer<C>::begin() const -> const_iterator {
    return base.begin();
}

template <typename C>
auto IndirectContainer<C>::cbegin() const noexcept -> const_iterator {
    return base.cbegin();
}

template <typename C>
auto IndirectContainer<C>::end() noexcept -> iterator {
    return base.end();
}

template <typename C>
auto IndirectContainer<C>::end() const noexcept -> const_iterator {
    return base.end();
}

template <typename C>
auto IndirectContainer<C>::cend() const noexcept -> const_iterator {
    return base.cend();
}

template <typename C>
auto IndirectContainer<C>::rbegin() -> reverse_iterator {
    return base.rbegin();
}

template <typename C>
auto IndirectContainer<C>::rbegin() const -> const_reverse_iterator {
    return base.rbegin();
}

template <typename C>
auto IndirectContainer<C>::crbegin() const noexcept -> const_reverse_iterator {
    return base.crbegin();
}

template <typename C>
auto IndirectContainer<C>::rend() -> reverse_iterator {
    return base.rend();
}

template <typename C>
auto IndirectContainer<C>::rend() const -> const_reverse_iterator {
    return base.rend();
}

template <typename C>
auto IndirectContainer<C>::crend() const noexcept -> const_reverse_iterator {
    return base.crend();
}

template <typename C>
auto IndirectContainer<C>::empty() const -> bool {
    return base.empty();
}

template <typename C>
auto IndirectContainer<C>::size() const -> size_type {
    return base.size();
}

template <typename C>
void IndirectContainer<C>::clear() {
    base.clear();
}
