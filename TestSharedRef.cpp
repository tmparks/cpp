#include "SharedRef.hpp"
#include "Verbose.hpp"
#include "compat/gsl14.hpp"

#include <gmock/gmock.h>
#include <vector>

using namespace testing;
using namespace testing::internal;

// Anonymous namespace for definitions that are local to this file.
namespace {
    template <typename T>
    using SharedContainer = std::vector<SharedRef<T>>;

#if __cplusplus >= 201703L

    // Create a smart reference to a newly constructed object of type T.
    // Note: copy elision is guaranteed since C++17
    template <typename T, typename... Args>
    SharedRef<T> create(Args&&... args) {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

#endif // C++17

    // Test that two objects are distinct (different addresses)
    bool distinct(const Verbose& a, const Verbose& b) { return &a != &b; }

    // Test that two objects are equal (same name)
    bool equal(const Verbose& a, const Verbose& b) {
        return a.name() == b.name();
    }

    // Print the elements of a container of smart references.
    void print(const SharedContainer<Verbose>& container) {
        for (const auto& elem : container) {
            std::cout << gsl::czstring { __func__ } << ": "
                      << elem // implicit conversion for operator<<
                      << std::endl;
            const auto& name = elem.get().name();
            EXPECT_THAT(name, Not(HasSubstr("copy")));
            EXPECT_THAT(name, Not(HasSubstr("move")));
        }
    }
} // anonymous namespace

TEST(SharedRef, constructor) {
#if __cplusplus >= 201703L
    auto a = create<Verbose>("one");
    auto b = create<Verbose>("two");
    // auto c = SharedRef<Verbose> { }; // no dangling references!
#else
    SharedRef<Verbose> a { std::make_shared<Verbose>("one") };
    SharedRef<Verbose> b { std::make_shared<Verbose>("two") };
    // SharedRef<Verbose> c { }; // no dangling references!
#endif // C++17

    EXPECT_FALSE(equal(a, b));
    EXPECT_TRUE(distinct(a, b));
}

TEST(SharedRef, copy) {
    // SharedRef is copyable because shared_ptr is copyable.
    EXPECT_TRUE(std::is_copy_constructible<SharedRef<int>>::value);
    EXPECT_TRUE(std::is_copy_assignable<SharedRef<int>>::value);

#if __cplusplus >= 201703L
    auto a = create<Verbose>("one");
    auto b = create<Verbose>("two");
    auto c = create<Verbose>("three");
    auto d = SharedRef<Verbose> { a }; // copy constructor
#else
    SharedRef<Verbose> a { std::make_shared<Verbose>("one") };
    SharedRef<Verbose> b { std::make_shared<Verbose>("two") };
    SharedRef<Verbose> c { std::make_shared<Verbose>("three") };
    SharedRef<Verbose> d { a }; // copy constructor
#endif // C++17

    c = b; // copy assignment

    // Copying modifies the reference, not the referenced object.

    EXPECT_TRUE(equal(a, d));
    EXPECT_FALSE(distinct(a, d)); // both refer to same object

    EXPECT_TRUE(equal(b, c));
    EXPECT_FALSE(distinct(b, c)); // both refer to same object

    const auto& cName = c.get().name();
    EXPECT_THAT(cName, EndsWith("two"));
    EXPECT_THAT(cName, Not(HasSubstr("three")));
    EXPECT_THAT(cName, Not(HasSubstr("copy")));
    EXPECT_THAT(cName, Not(HasSubstr("move")));
    EXPECT_THAT(cName, Not(HasSubstr("construct")));
    EXPECT_THAT(cName, Not(HasSubstr("assign")));

    const auto& dName = d.get().name();
    EXPECT_THAT(dName, EndsWith("one"));
    EXPECT_THAT(dName, Not(HasSubstr("copy")));
    EXPECT_THAT(dName, Not(HasSubstr("move")));
    EXPECT_THAT(dName, Not(HasSubstr("construct")));
    EXPECT_THAT(dName, Not(HasSubstr("assign")));
}

TEST(SharedRef, move) {
    // SharedRef is not moveable because the moved-from shared_ptr would be null.
    EXPECT_FALSE(std::is_move_constructible<SharedRef<int>>::value);
    EXPECT_FALSE(std::is_move_assignable<SharedRef<int>>::value);
}

TEST(SharedRef, reset) {
    auto&& a = SharedRef<Verbose> { std::make_shared<Verbose>("one") }; // C++17
    a.reset(std::make_shared<Verbose>("two"));
    EXPECT_EQ("two", a.get().name());
}

TEST(SharedRef, swap) {
    auto&& a = SharedRef<Verbose> { std::make_shared<Verbose>("one") }; // C++17
    auto&& b = SharedRef<Verbose> { std::make_shared<Verbose>("two") }; // C++17
    swap(a, b);
    EXPECT_EQ("one", b.get().name());
    EXPECT_EQ("two", a.get().name());
}

TEST(SharedRef, container) {
    auto container = SharedContainer<Verbose> {};
    container.emplace_back(std::make_shared<Verbose>("one"));
    container.emplace_back(std::make_shared<Verbose>("two"));
    container.emplace_back(std::make_shared<Verbose>("three"));

    print(container);

    for (const auto& elem : container) {
        std::cout << elem.get().name() << std::endl; // explicitly get reference
    }

    for (const Verbose& elem : container) { // implicit conversion (cannot use auto)
        std::cout << elem.name() << std::endl;
    }

    // Copying a container of shared references does not copy the referenced objects.
    auto container_copy = container;
    print(container_copy);
}
