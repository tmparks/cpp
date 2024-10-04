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
