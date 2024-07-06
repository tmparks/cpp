#include "SharedRef.hpp"
#include "Verbose.hpp"
#include "compat/gsl14.hpp"

#include <gmock/gmock.h>

using namespace testing;
using namespace testing::internal;

// Anonymous namespace for definitions that are local to this file.
namespace {
    template <typename T>
    using SharedVector = std::vector<SharedRef<T>>;

#if __cplusplus >= 201703L

    // Create a shared reference to a newly constructed object of type T.
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

    // Print the elements of a vector of shared references.
    void print(const SharedVector<Verbose>& v) {
        for (const auto& elem : v) {
            std::cout << gsl::czstring(__func__) << ": "
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

    c = b;                             // copy assignment

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

TEST(SharedRef, container) {
    SharedVector<Verbose> v1 { {
        { std::make_shared<Verbose>("one") },
        { std::make_shared<Verbose>("two") },
        { std::make_shared<Verbose>("three") } } };

    print(v1);

    for (const auto& elem : v1) {
        std::cout << elem.get().name() << std::endl; // explicitly get reference
    }

    for (const Verbose& elem : v1) { // implicit conversion (cannot use auto)
        std::cout << elem.name() << std::endl;
    }

    // Copying a vector of shared references does not copy the referenced objects.
    auto v2 = v1;
    print(v2);
}
