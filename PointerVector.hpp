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

////////////////////////////////////////////////////////////////////////////////

template <typename P>
class PointerVector : public std::vector<P> {
private:
    using base = std::vector<P>;

public:
    using value_type = std::pointer_traits<P>::element_type;
    using size_type = base::size_type;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = P;
    using iterator =
            boost::indirect_iterator<typename base::iterator>;
    using const_iterator =
            boost::indirect_iterator<typename base::const_iterator>;
    using reverse_iterator =
            boost::indirect_iterator<typename base::reverse_iterator>;
    using const_reverse_iterator =
            boost::indirect_iterator<typename base::const_reverse_iterator>;

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
    PointerVector(base::size_type, base::const_reference);
    explicit PointerVector(base::size_type);
    base::iterator insert(
            base::const_iterator, base::size_type, base::const_reference);
    using base::data;
    using base::resize;
};
