#pragma once

#include <boost/iterator/indirect_iterator.hpp>

template <typename S>
class IndirectSequence {
public:
    using value_type =
            typename std::pointer_traits<typename S::value_type>::element_type;
    using size_type = typename S::size_type;
    using difference_type = typename S::difference_type;
    using reference = value_type&;
    using const_reference = const value_type&;
    using iterator = boost::indirect_iterator<typename S::iterator>;
    using const_iterator = boost::indirect_iterator<typename S::const_iterator>;
    using reverse_iterator =
            boost::indirect_iterator<typename S::reverse_iterator>;
    using const_reverse_iterator =
            boost::indirect_iterator<typename S::const_reverse_iterator>;

    S base{};
    bool operator==(const IndirectSequence& other) const noexcept;
    bool operator<(const IndirectSequence& other) const noexcept;

    reference at(size_type pos);
    [[nodiscard]] const_reference at(size_type pos) const;

    reference operator[](size_type pos);
    const_reference operator[](size_type pos) const;

    reference front();
    [[nodiscard]] const_reference front() const;

    reference back();
    [[nodiscard]] const_reference back() const;

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

    [[nodiscard]] bool empty() const;
    [[nodiscard]] size_type size() const;
    void clear();
};

////////////////////////////////////////////////////////////////////////////////

template <typename S>
auto IndirectSequence<S>::operator==(const IndirectSequence& other) const noexcept
        -> bool {
    return base == other.base;
}

template <typename S>
auto IndirectSequence<S>::operator<(const IndirectSequence& other) const noexcept
        -> bool {
    return base < other.base;
}

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

template <typename S>
auto IndirectSequence<S>::begin() -> iterator {
    return base.begin();
}

template <typename S>
auto IndirectSequence<S>::begin() const -> const_iterator {
    return base.begin();
}

template <typename S>
auto IndirectSequence<S>::cbegin() const noexcept -> const_iterator {
    return base.cbegin();
}

template <typename S>
auto IndirectSequence<S>::end() noexcept -> iterator {
    return base.end();
}

template <typename S>
auto IndirectSequence<S>::end() const noexcept -> const_iterator {
    return base.end();
}

template <typename S>
auto IndirectSequence<S>::cend() const noexcept -> const_iterator {
    return base.cend();
}

template <typename S>
auto IndirectSequence<S>::rbegin() -> reverse_iterator {
    return base.rbegin();
}

template <typename S>
auto IndirectSequence<S>::rbegin() const -> const_reverse_iterator {
    return base.rbegin();
}

template <typename S>
auto IndirectSequence<S>::crbegin() const noexcept -> const_reverse_iterator {
    return base.crbegin();
}

template <typename S>
auto IndirectSequence<S>::rend() -> reverse_iterator {
    return base.rend();
}

template <typename S>
auto IndirectSequence<S>::rend() const -> const_reverse_iterator {
    return base.rend();
}

template <typename S>
auto IndirectSequence<S>::crend() const noexcept -> const_reverse_iterator {
    return base.crend();
}

template <typename S>
auto IndirectSequence<S>::empty() const -> bool {
    return base.empty();
}

template <typename S>
auto IndirectSequence<S>::size() const -> size_type {
    return base.size();
}

template <typename S>
void IndirectSequence<S>::clear() {
    base.clear();
}
