#pragma once

#include <boost/iterator/indirect_iterator.hpp>
#include <memory>
#include <vector>

template <typename P>
class PointerVector;

template <typename T>
using RawPointerVector = PointerVector<T*>;

template <typename T>
using UniquePointerVector = PointerVector<std::unique_ptr<T>>;

template <typename T>
using SharedPointerVector = PointerVector<std::shared_ptr<T>>;

template <typename S>
RawPointerVector<typename S::value_type> reference(S& sequence) {
    auto result = RawPointerVector<typename S::value_type>{};
    result.reserve(sequence.size());
    for (auto& element : sequence) {
        result.push_pack(&element);
    }
    return result;
}

template <typename S>
const RawPointerVector<typename S::value_type> reference(const S& sequence) {
    return const_cast<const RawPointerVector<typename S::value_type>>(
            reference(const_cast<S&>(sequence))); // NOLINT(*-const-cast)
}

template <typename S>
RawPointerVector<typename S::value_type> mask(S& sequence, std::vector<bool>& mask) {
    auto result = RawPointerVector<typename S::value_type>{};
    auto size = std::min(sequence.size(), mask.size());
    result.reserve(size);
    for (typename S::size_type i = 0; i < size; i++) {
        if (mask[i]) {
            result.push_back(&sequence[i]);
        }
    }
    return result;
}

template <typename S>
const RawPointerVector<typename S::value_type> mask(
        const S& sequence, std::vector<bool>& mask) {
    return const_cast<const RawPointerVector<typename S::value_type>>(
            mask(const_cast<S&>(sequence), mask)); // NOLINT(*-const-cast)
}

////////////////////////////////////////////////////////////////////////////////

template <typename P>
class PointerVector {
public:
    using base_type = typename std::vector<P>;
    using value_type = typename std::pointer_traits<P>::element_type;
    using size_type = typename base_type::size_type;
    using difference_type = typename base_type::difference_type;
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

    bool operator==(const PointerVector& other) const noexcept {
        return base == other.base;
    }

    bool operator<(const PointerVector& other) const noexcept {
        return base < other.base;
    }

    // Element access

    [[nodiscard]] reference at(size_type pos) { return *base.at(pos); }

    [[nodiscard]] const_reference at(size_type pos) const {
        return *base.at(pos);
    }

    [[nodiscard]] reference operator[](size_type pos) { return *base[pos]; }

    [[nodiscard]] const_reference operator[](size_type pos) const {
        return *base[pos];
    }

    [[nodiscard]] reference front() { return *base.front(); }

    [[nodiscard]] const_reference front() const { return *base.front(); }

    [[nodiscard]] reference back() { return *base.back(); }

    [[nodiscard]] const_reference back() const { return *base.back(); }

    // Iterators

    [[nodiscard]] iterator begin() { return iterator{base.begin()}; }

    [[nodiscard]] const_iterator begin() const {
        return const_iterator{base.begin()};
    }

    [[nodiscard]] const_iterator cbegin() const noexcept {
        return const_iterator{base.cbegin()};
    }

    [[nodiscard]] iterator end() noexcept { return iterator{base.end()}; }

    [[nodiscard]] const_iterator end() const noexcept {
        return const_iterator{base.end()};
    }

    [[nodiscard]] const_iterator cend() const noexcept {
        return const_iterator{base.cend()};
    }

    [[nodiscard]] reverse_iterator rbegin() {
        return reverse_iterator{base.rbegin()};
    }

    [[nodiscard]] const_reverse_iterator rbegin() const {
        return const_reverse_iterator{base.rbegin()};
    }

    [[nodiscard]] const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator{base.crbegin()};
    }

    [[nodiscard]] reverse_iterator rend() {
        return reverse_iterator{base.rend()};
    }

    [[nodiscard]] const_reverse_iterator rend() const {
        return const_reverse_iterator{base.rend()};
    }

    [[nodiscard]] const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator{base.crend()};
    }

    // Capacity

    [[nodiscard]] bool empty() const { return base.empty(); }

    [[nodiscard]] size_type size() const { return base.size(); }

    void reserve(size_type cap) { base.reserve(cap); }

    [[nodiscard]] size_type capacity() const { return base.capacity(); }

    // Modifiers

    void clear() { base.clear(); }
};
