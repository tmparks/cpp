#include "UniqueRef.hpp"
#include "Verbose.hpp"
#include "compat/gsl14.hpp"

#include <gmock/gmock.h>
#include <list>

using namespace testing;
using namespace testing::internal;

// Anonymous namespace for definitions that are local to this file.
namespace {
    // UniqueRef is neither copyable nor movable, so we use std::list instead
    // of std::vector, which sometimes needs to reallocate its storage.
    template <typename T>
    using UniqueContainer = std::list<UniqueRef<T>>;

#if __cplusplus >= 201703L // since C++17

    // Create a smart reference to a newly constructed object of type T.
    // Note: copy elision is guaranteed since C++17
    template <typename T, typename... Args>
    UniqueRef<T> create(Args&&... args) {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

#endif // C++17

    // Test that two objects are distinct (different addresses)
    bool distinct(const Verbose<>& a, const Verbose<>& b) { return &a != &b; }

    // Test that two objects are equal (same name)
    bool equal(const Verbose<>& a, const Verbose<>& b) {
        return a.name() == b.name();
    }

    // Print the elements of a container of smart references.
    void print(const UniqueContainer<Verbose<>>& container) {
        for (const auto& elem : container) {
            std::cout << gsl::czstring{__func__} << ": " << elem.get()
                      << std::endl;
            const auto& name = elem.get().name();
            EXPECT_THAT(name, Not(HasSubstr("copy")));
            EXPECT_THAT(name, Not(HasSubstr("move")));
        }
    }
} // anonymous namespace

TEST(UniqueRef, constructor) {
#if __cplusplus >= 201703L // since C++17
    auto a = create<Verbose<>>("one");
    auto b = create<Verbose<>>("two");
    // auto c = UniqueRef<Verbose<>> { }; // no dangling references!
#else // until C++17
    UniqueRef<Verbose<>> a{std::make_unique<Verbose<>>("one")};
    UniqueRef<Verbose<>> b{std::make_unique<Verbose<>>("two")};
    // UniqueRef<Verbose<>> c { }; // no dangling references!
#endif // C++17

    EXPECT_FALSE(equal(a, b));
    EXPECT_TRUE(distinct(a, b));
}

TEST(UniqueRef, copy) {
    // UniqueRef is not copyable because unique_ptr is not copyable.
    EXPECT_FALSE(std::is_copy_constructible<UniqueRef<int>>::value);
    EXPECT_FALSE(std::is_copy_assignable<UniqueRef<int>>::value);
}

TEST(UniqueRef, move) {
    // UniqueRef is not moveable because the moved-from unique_ptr would be null.
    EXPECT_FALSE(std::is_move_constructible<UniqueRef<int>>::value);
    EXPECT_FALSE(std::is_move_assignable<UniqueRef<int>>::value);
}

TEST(UniqueRef, reset) {
    UniqueRef<Verbose<>> a{std::make_unique<Verbose<>>("one")};
    a.reset(std::make_unique<Verbose<>>("two"));
    EXPECT_EQ("two", a.get().name());
}

TEST(UniqueRef, swap) {
    UniqueRef<Verbose<>> a{std::make_unique<Verbose<>>("one")};
    UniqueRef<Verbose<>> b{std::make_unique<Verbose<>>("two")};
    swap(a, b);
    EXPECT_EQ("one", b.get().name());
    EXPECT_EQ("two", a.get().name());
}

TEST(UniqueRef, container) {
    UniqueContainer<Verbose<>> container;
    container.emplace_back(std::make_unique<Verbose<>>("one"));
    container.emplace_back(std::make_unique<Verbose<>>("two"));
    container.emplace_back(std::make_unique<Verbose<>>("three"));

    print(container);

    for (const auto& elem : container) {
        // explicitly get reference
        std::cout << elem.get().name() << std::endl;
    }

    // implicit conversion (cannot use auto)
    for (const Verbose<>& elem : container) {
        std::cout << elem.name() << std::endl;
    }
}
