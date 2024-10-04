#pragma once

#include "IndirectSequence.hpp"
#include <memory>
#include <vector>

template <typename P>
using PointerVector = IndirectSequence<std::vector<P>>;

template <typename T>
using RawPointerVector = PointerVector<T*>;

template <typename T>
using UniquePointerVector = PointerVector<std::unique_ptr<T>>;

template <typename T>
using SharedPointerVector = PointerVector<std::shared_ptr<T>>;

template <typename S>
RawPointerVector<typename S::value_type> reference(S& sequence) {
    auto result = RawPointerVector<typename S::value_type>{};
    result.base.reserve(sequence.size());
    for (auto& element : sequence) {
        result.base.emplace_back(&element);
    }
    return result;
}

template <typename S>
const RawPointerVector<typename S::value_type> reference(const S& sequence) {
    return const_cast<const RawPointerVector<typename S::value_type>>(
            reference(const_cast<S&>(sequence))); // NOLINT(*-const-cast)
}

template <typename S>
RawPointerVector<typename S::value_type> mask(
        S& sequence, const std::vector<bool>& mask) {
    auto result = RawPointerVector<typename S::value_type>{};
    auto size = std::min(sequence.size(), mask.size());
    result.base.reserve(size);
    for (typename S::size_type i = 0; i < size; i++) {
        if (mask[i]) {
            result.base.push_back(&sequence[i]);
        }
    }
    return result;
}

template <typename S>
const RawPointerVector<typename S::value_type> mask(
        const S& sequence, const std::vector<bool>& mask) {
    return const_cast<const RawPointerVector<typename S::value_type>>(
            mask(const_cast<S&>(sequence), mask)); // NOLINT(*-const-cast)
}
