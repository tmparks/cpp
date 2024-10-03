#pragma once

#include <boost/iterator/indirect_iterator.hpp>
#include <map>
#include <memory>

template <typename Key, typename P>
class PointerMap;

template <typename Key, typename T>
using RawPointerMap = PointerMap<Key, T*>;

template <typename Key, typename T>
using UniquePointerMap = PointerMap<Key, std::unique_ptr<T>>;

template <typename Key, typename T>
using SharedPointerMap = PointerMap<Key, std::shared_ptr<T>>;

////////////////////////////////////////////////////////////////////////////////

template <typename Key, typename P>
class PointerMap {
public:
    using base_type = typename std::map<Key, P>;
    using key_type = typename base_type::key_type;
    using mapped_type = typename std::pointer_traits<P>::element_type;
    using value_type = typename std::pair<const key_type, mapped_type>;
    using size_type = typename base_type::size_type;
    using difference_type = typename base_type::difference_type;
    using key_compare = typename base_type::key_compare;
    using reference = value_type&;
    using const_reference = const value_type&;
    using iterator =
            boost::indirect_iterator<typename base_type::iterator>;
    using const_iterator =
            boost::indirect_iterator<typename base_type::const_iterator>;
    using reverse_iterator =
            boost::indirect_iterator<typename base_type::reverse_iterator>;
    using const_reverse_iterator =
            boost::indirect_iterator<typename base_type::const_reverse_iterator>;

    // Base access

    base_type base{};

    // Comparison

    bool operator==(const PointerMap& other) const noexcept {
        return base == other.base;
    }

    bool operator<(const PointerMap& other) const noexcept {
        return base < other.base;
    }

    // Element access

    reference at(const key_type& key) { return *base.at(key); }

    const_reference at(const key_type& key) const { return *base.at(key); }

    reference operator[](const key_type& key) { return *base[key]; }

    reference operator[](key_type&& key) { return *base[std::move(key)]; }

    // Iterators

    iterator begin() { return iterator{base.begin()}; }

    const_iterator begin() const { return const_iterator{base.begin()}; }

    const_iterator cbegin() const noexcept {
        return const_iterator{base.cbegin()};
    }

    iterator end() noexcept { return iterator{base.end()}; }

    const_iterator end() const noexcept { return const_iterator{base.end()}; }

    const_iterator cend() const noexcept { return const_iterator{base.cend()}; }

    reverse_iterator rbegin() { return reverse_iterator{base.rbegin()}; }

    const_reverse_iterator rbegin() const {
        return const_reverse_iterator{base.rbegin()};
    }

    const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator{base.crbegin()};
    }

    reverse_iterator rend() { return reverse_iterator{base.rend()}; }

    const_reverse_iterator rend() const {
        return const_reverse_iterator{base.rend()};
    }

    const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator{base.crend()};
    }

    // Capacity

    [[nodiscard]] bool empty() const { return base.empty(); }

    size_type size() const { return base.size(); }

    void reserve(size_type cap) { base.reserve(cap); }

    size_type capacity() const { return base.capacity(); }

    // Modifiers

    void clear() { base.clear(); }

    // Lookup

    size_type count(const key_type& key) const { return base.count(key); }

    iterator find(const key_type& key) { return iterator{base.find(key)}; }

    bool contains(const key_type& key) const { return base.contains(key); }

    std::pair<iterator, iterator> equal_range(const key_type& key) {
        auto&& result = base.equal_range(key);
        return std::make_pair(iterator{result.first}, iterator{result.second});
    }

    iterator lower_bound(const key_type& key) {
        return iterator{base.lower_bound(key)};
    }

    const_iterator lower_bound(const key_type& key) const {
        return const_iterator{base.lower_bound(key)};
    }

    iterator upper_bound(const key_type& key) {
        return iterator{base.upper_bound(key)};
    }

    const_iterator upper_bound(const key_type& key) const {
        return const_iterator{base.upper_bound(key)};
    }
};
