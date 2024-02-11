#include "Verbose.hpp"

#include "compat/gsl14.hpp"

#include <cassert>
#include <mutex>
#include <thread>

// See https://en.cppreference.com/w/cpp/named_req/BasicLockable
template <typename T>
class BasicLockable : protected Verbose {
public:
  BasicLockable() : Verbose {gsl::czstring(__func__)} {}

  void lock() {
    lock_.lock();
#ifndef NDEBUG
    owner_ = std::this_thread::get_id();
#endif // NDEBUG
  }

  void unlock() {
#ifndef NDEBUG
    owner_ = none_;
#endif // NDEBUG
    lock_.unlock();
  }

  void assert_ownership() {
#ifndef NDEBUG
    assert((owner_ == std::this_thread::get_id()) &&
           "Lock should be owned by this thread");
#endif // NDEBUG
  }

#ifdef NDEBUG
  constexpr bool owner_is_this_thread() {
    return true; // never fails for RELEASE build
  }
#else
  bool owner_is_this_thread() {
    return owner_ == std::this_thread::get_id();
  }
#endif // NDEBUG

private:
#ifndef NDEBUG
  static const std::thread::id none_;
  std::thread::id owner_ {none_}; // initially unowned
#endif // NDEBUG
  T lock_;                        // initially unlocked
};

#ifndef NDEBUG
template <typename T>
const std::thread::id BasicLockable<T>::none_ {};
#endif // NDEBUG

////////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>
#include <iostream>

using namespace testing;
using namespace testing::internal;

#if __cplusplus >= 201703L

TEST(BasicLockable, lock_guard) {
  auto mutex = BasicLockable<std::mutex>();
  auto lock = std::lock_guard(mutex);
  mutex.assert_ownership();
}

TEST(BasicLockable, unique_lock) {
  auto mutex = BasicLockable<std::mutex>();
  auto lock = std::unique_lock(mutex);
  mutex.assert_ownership();
}

#endif // C++17

// Does not fail for RELEASE build
TEST(BasicLockable, unowned_expects) {
#ifdef NDEBUG
  auto mutex = BasicLockable<std::mutex>();
  Expects(mutex.owner_is_this_thread());
#endif // NDEBUG
}

// Does not abort for RELEASE build
TEST(BasicLockable, unowned_assert) {
#ifdef NDEBUG
  auto mutex = BasicLockable<std::mutex>();
  mutex.assert_ownership(); // Expected to fail!
#endif // NDEBUG
}

#if __cplusplus >= 201703L

TEST(BasicLockable, timing_assert) {
  const auto limit = 10'000'000;
  auto mutex = BasicLockable<std::mutex>();
  for (auto i = 0; i < limit; i++) {
    auto lock = std::lock_guard(mutex);
    mutex.assert_ownership();
  }
}

TEST(BasicLockable, timing_expects) {
  const auto limit = 10'000'000;
  auto mutex = BasicLockable<std::mutex>();
  for (auto i = 0; i < limit; i++) {
    auto lock = std::lock_guard(mutex);
    Expects(mutex.owner_is_this_thread());
  }
}

TEST(BasicLockable, timing_without) {
  const auto limit = 10'000'000;
  auto mutex = std::mutex();
  for (auto i = 0; i < limit; i++) {
    auto lock = std::lock_guard(mutex);
  }
}

#endif // C++17
