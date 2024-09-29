#pragma once

#include <boost/iterator/indirect_iterator.hpp>
#include <memory>
#include <vector>

template <typename Value, typename Pointer>
class PointerVector;

template <typename T>
using RawPointerVector = PointerVector<T, T*>;

template <typename T>
using UniquePointerVector = PointerVector<T, std::unique_ptr<T>>;

template <typename T>
using SharedPointerVector = PointerVector<T, std::shared_ptr<T>>;

////////////////////////////////////////////////////////////////////////////////

template <typename Value, typename Pointer>
class PointerVector : public std::vector<Pointer> {
private:
    using base = std::vector<Pointer>;

public:
    using value_type = Value;
    using size_type = base::size_type;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = Pointer;
    using iterator = boost::indirect_iterator<typename base::iterator>;
    using const_iterator =
            boost::indirect_iterator<typename base::const_iterator>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    using base::base;
    ~PointerVector() = default;

    [[nodiscard]] reference at(size_type pos) { return *base::at(pos); }
    [[nodiscard]] const_reference at(size_type pos) const {
        return *base::at(pos);
    }

    [[nodiscard]] reference operator[](size_type pos) {
        return *base::operator[](pos);
    }
    [[nodiscard]] const_reference operator[](size_type pos) const {
        return *base::operator[](pos);
    }

    [[nodiscard]] reference front() { return *base::front(); }
    [[nodiscard]] const_reference front() const { return *base::front(); }

    [[nodiscard]] reference back() { return *base::back(); }
    [[nodiscard]] const_reference back() const { return *base::back(); }

    [[nodiscard]] iterator begin() { return iterator{base::begin()}; }
    [[nodiscard]] const_iterator begin() const {
        return const_iterator{base::begin()};
    }
    [[nodiscard]] const_iterator cbegin() const noexcept {
        return const_iterator{base::cbegin()};
    }

    [[nodiscard]] iterator end() noexcept { return iterator{base::end()}; }
    [[nodiscard]] const_iterator end() const noexcept {
        return const_iterator{base::end()};
    }
    [[nodiscard]] const_iterator cend() const noexcept {
        return const_iterator{base::cend()};
    }

    [[nodiscard]] reverse_iterator rbegin() {
        return reverse_iterator{base::rbegin()};
    }
    [[nodiscard]] const_reverse_iterator rbegin() const {
        return const_reverse_iterator{base::rbegin()};
    }
    [[nodiscard]] const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator{base::crbegin()};
    }

    [[nodiscard]] reverse_iterator rend() {
        return reverse_iterator{base::rend()};
    }
    [[nodiscard]] const_reverse_iterator rend() const {
        return const_reverse_iterator{base::rend()};
    }
    [[nodiscard]] const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator{base::crend()};
    }

private:
    PointerVector(size_type, const_reference);
    explicit PointerVector(size_type);
    using base::data;
    using base::resize;
};
