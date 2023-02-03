#include "Verbose.hpp"

#include <gsl/gsl>

#include <atomic>
#include <cassert>
#include <mutex>
#include <thread>

// See https://en.cppreference.com/w/cpp/named_req/BasicLockable
template <typename T> class BasicLockable : protected Verbose {
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

  void this_thread_is_owner() {
#ifndef NDEBUG
    if (owner_ != std::this_thread::get_id()) {
      throw std::runtime_error("Lock is not owned by this thread!");
    }
#endif // NDEBUG
  }

private:
#ifndef NDEBUG
  static const std::thread::id none_;
  std::atomic<std::thread::id> owner_ {none_}; // initially unowned
#endif // NDEBUG
  T lock_;                                     // initially unlocked
};

#ifndef NDEBUG
template <typename T> const std::thread::id BasicLockable<T>::none_ {};
#endif // NDEBUG

////////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>
#include <iostream>

using namespace testing;
using namespace testing::internal;

TEST(BasicLockable, lock_guard) {
  auto mutex = BasicLockable<std::mutex>();
  auto lock = std::lock_guard<decltype(mutex)>(mutex);
  mutex.assert_ownership();
}

TEST(BasicLockable, unique_lock) {
  auto mutex = BasicLockable<std::mutex>();
  auto lock = std::unique_lock<decltype(mutex)>(mutex);
  mutex.assert_ownership();
}

// Does not throw for RELEASE build
TEST(BasicLockable, unowned_throw) {
  auto mutex = BasicLockable<std::mutex>();
#ifdef NDEBUG
  EXPECT_NO_THROW(mutex.this_thread_is_owner());
#else
  EXPECT_THROW(mutex.this_thread_is_owner(), std::runtime_error);
#endif // NDEBUG
}

// Does not abort for RELEASE build
TEST(BasicLockable, unowned_assert) {
#ifdef NDEBUG
  auto mutex = BasicLockable<std::mutex>();
  mutex.assert_ownership(); // Expected to fail!
#endif // NDEBUG
}
