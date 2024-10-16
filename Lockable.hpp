#include "compat/gsl14.hpp"
#ifndef NDEBUG
#include <cassert>
#include <thread>
#endif // NDEBUG

// See https://en.cppreference.com/w/cpp/named_req/BasicLockable
template <typename T>
class BasicLockable {
public:
    void lock();
    void unlock();
    void assert_ownership() const;
    [[nodiscard]] bool this_thread_is_owner() const;

private:
#ifndef NDEBUG
    static const std::thread::id none_; // initialized later
    std::thread::id owner_{none_};      // initially unowned
#endif // NDEBUG
    T lock_{};                          // initially unlocked
};

////////////////////////////////////////////////////////////////////////////////

template <typename T>
void BasicLockable<T>::lock() {
    lock_.lock();
#ifndef NDEBUG
    owner_ = std::this_thread::get_id();
#endif // NDEBUG
}

template <typename T>
void BasicLockable<T>::unlock() {
#ifndef NDEBUG
    owner_ = none_;
#endif // NDEBUG
    lock_.unlock();
}

template <typename T>
void BasicLockable<T>::assert_ownership() const {
#ifdef NDEBUG
    // never fails for RELEASE build
#else
    assert((owner_ == std::this_thread::get_id())
           && "Lock should be owned by this thread");
#endif // NDEBUG
}

template <typename T>
bool BasicLockable<T>::this_thread_is_owner() const {
#ifdef NDEBUG
    return true; // never fails for RELEASE build
#else
    return std::this_thread::get_id() == owner_;
#endif // NDEBUG
}

#ifndef NDEBUG
template <typename T>
const std::thread::id BasicLockable<T>::none_{};
#endif // NDEBUG
