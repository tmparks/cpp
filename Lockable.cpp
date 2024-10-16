#include "Lockable.hpp"
#include "Verbose.hpp"
#include "compat/gsl14.hpp"
#include <gtest/gtest.h>
#include <mutex>

using namespace testing;
using namespace testing::internal;

using VerboseMutex = Verbose<BasicLockable<std::mutex>>;

TEST(BasicLockable, lock_guard) {
    auto&& mutex = VerboseMutex{"mutex"};               // && until C++17
    auto&& lock = std::lock_guard<VerboseMutex>{mutex}; // && until C++17
    mutex.assert_ownership();
}

TEST(BasicLockable, unique_lock) {
    auto&& mutex = VerboseMutex{"mutex"};                // && until C++17
    auto&& lock = std::unique_lock<VerboseMutex>{mutex}; // && until C++17
    mutex.assert_ownership();
}

// Does not fail for RELEASE build
TEST(BasicLockable, unowned_expects) {
#ifdef NDEBUG
    auto&& mutex = VerboseMutex{"mutex"};  // && until C++17
    Expects(mutex.this_thread_is_owner()); // Expected to fail!
#endif // NDEBUG
}

// Does not abort for RELEASE build
TEST(BasicLockable, unowned_assert) {
#ifdef NDEBUG
    auto&& mutex = VerboseMutex{"mutex"}; // && until C++17
    mutex.assert_ownership();             // Expected to fail!
#endif // NDEBUG
}

#if __cplusplus >= 201703L // since C++17

TEST(BasicLockable, timing_assert) {
#ifdef NDEBUG
    const auto limit = 100'000'000;
#else
    const auto limit = 100'000;
#endif // NDEBUG
    auto mutex = VerboseMutex{"mutex"};
    for (auto i = 0; i < limit; i++) {
        auto lock = std::lock_guard{mutex};
        mutex.assert_ownership();
    }
}

TEST(BasicLockable, timing_expects) {
#ifdef NDEBUG
    const auto limit = 100'000'000;
#else
    const auto limit = 100'000;
#endif // NDEBUG
    auto mutex = VerboseMutex{"mutex"};
    for (auto i = 0; i < limit; i++) {
        auto lock = std::lock_guard{mutex};
        Expects(mutex.this_thread_is_owner());
    }
}

TEST(BasicLockable, timing_without) {
    const auto limit = 100'000'000;
    auto mutex = std::mutex{};
    for (auto i = 0; i < limit; i++) {
        auto lock = std::lock_guard{mutex};
    }
}

#endif // C++17
