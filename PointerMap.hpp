#pragma once

#include "IndirectMap.hpp"
#include <map>
#include <memory>

template <typename Key, typename P>
using PointerMap = IndirectMap<std::map<Key, P>>;

template <typename Key, typename T>
using RawPointerMap = PointerMap<Key, T*>;

template <typename Key, typename T>
using UniquePointerMap = PointerMap<Key, std::unique_ptr<T>>;

template <typename Key, typename T>
using SharedPointerMap = PointerMap<Key, std::shared_ptr<T>>;

template <typename M>
RawPointerMap<typename M::key_type, typename M::mapped_type> reference([[maybe_unused]] M& map) {
    auto result = RawPointerMap<typename M::key_type, typename M::mapped_type>{};
    for (auto&& element : map) {
        result.base.emplace(element.first, &element.second);
    }
    return result;
}

template <typename M>
const RawPointerMap<typename M::key_type, typename M::mapped_type> reference(
        const M& map) {
    return const_cast<const RawPointerMap<typename M::key_type, typename M::mapped_type>>(
            reference(const_cast<M&>(map))); // NOLINT(*-const-cast)
}
