#include "Verbose.hpp"

#include <gsl/gsl>

#include <atomic>
#include <cassert>
#include <mutex>
#include <thread>

// See https://en.cppreference.com/w/cpp/named_req/BasicLockable
class BasicLockable : protected Verbose {
public:
  BasicLockable() : Verbose {gsl::czstring(__func__)} {}

  void lock() {
    mutex_.lock();
    owner_ = std::this_thread::get_id();
  }

  void unlock() {
    owner_ = none_;
    mutex_.unlock();
  }

  void assert_ownership() {
    assert((owner_ == std::this_thread::get_id()) &&
           "Lock is owned by this thread");
  }

  void this_thread_is_owner() {
#ifndef NDEBUG
    if (owner_ != std::this_thread::get_id()) {
      throw std::runtime_error("Lock is not owned by this thread!");
    }
#endif
  }

private:
  static const std::thread::id none_; // id that is not associated with any thread
  std::atomic<std::thread::id> owner_ {none_}; // initially unowned
  std::mutex mutex_; // initially unlocked
};

const std::thread::id BasicLockable::none_ {};

////////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>
#include <iostream>

using namespace testing;
using namespace testing::internal;

TEST(BasicLockable, lock_guard) {
  auto mutex = BasicLockable();
  auto lock = std::lock_guard<decltype(mutex)>(mutex);
  mutex.assert_ownership();
}

TEST(BasicLockable, unique_lock) {
  auto mutex = BasicLockable();
  auto lock = std::unique_lock<decltype(mutex)>(mutex);
  mutex.assert_ownership();
}

TEST(BasicLockable, unowned_throw) {
  auto mutex = BasicLockable();
#ifdef NDEBUG
  EXPECT_NO_THROW(mutex.this_thread_is_owner());
#else
  EXPECT_THROW(mutex.this_thread_is_owner(), std::runtime_error);
#endif // NDEBUG
}

TEST(BasicLockable, DISABLED_unowned_throw_unhandled) {
  auto mutex = BasicLockable();
  mutex.this_thread_is_owner();
}

TEST(BasicLockable, DISABLED_unowned_assert) {
  auto mutex = BasicLockable();
  mutex.assert_ownership(); // Expected to fail!
}
