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

    // Test that two objects are distinct (different addresses)
    bool distinct(const Verbose<>& a, const Verbose<>& b) { return &a != &b; }

    // Test that two objects are equal (same name)
    bool equal(const Verbose<>& a, const Verbose<>& b) {
        return a.name() == b.name();
    }

    // Print the elements of a container of smart references.
    void print(const SharedContainer<Verbose<>>& container) {
        for (const Verbose<>& elem : container) {
            std::cout << gsl::czstring{__func__} << ": " << elem << std::endl;
            const auto& name = elem.name();
            EXPECT_THAT(name, Not(HasSubstr("copy")));
            EXPECT_THAT(name, Not(HasSubstr("move")));
        }
    }
} // anonymous namespace

TEST(SharedRef, constructor) {
    auto a = makeSharedRef<Verbose<>>("one");
    auto b = makeSharedRef<Verbose<>>("two");
    // auto c = SharedRef<Verbose<>> { }; // no dangling references!

    EXPECT_FALSE(equal(a, b));
    EXPECT_TRUE(distinct(a, b));
}

TEST(SharedRef, copy) {
    // SharedRef is copyable because shared_ptr is copyable.
    EXPECT_TRUE(std::is_copy_constructible<SharedRef<int>>::value);
    EXPECT_TRUE(std::is_copy_assignable<SharedRef<int>>::value);

    auto a = makeSharedRef<Verbose<>>("one");
    auto b = makeSharedRef<Verbose<>>("two");
    auto c = makeSharedRef<Verbose<>>("three");
    auto d = SharedRef<Verbose<>>{a}; // copy constructor

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
    // SharedRef is moveable because shared_ptr is moveable.
    EXPECT_TRUE(std::is_move_constructible<SharedRef<int>>::value);
    EXPECT_TRUE(std::is_move_assignable<SharedRef<int>>::value);

    auto a = makeSharedRef<Verbose<>>("one");
    auto b = makeSharedRef<Verbose<>>("two");
    auto c = makeSharedRef<Verbose<>>("three");
    auto d = SharedRef<Verbose<>>{std::move(a)}; // move constructor

    c = std::move(b); // move assignment

    // Moving modifies the reference, not the referenced object.

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

TEST(SharedRef, reset) {
    auto a = makeSharedRef<Verbose<>>("one");
    a.reset(std::make_shared<Verbose<>>("two"));
    EXPECT_EQ("two", a.get().name());
}

TEST(SharedRef, swap) {
    auto a = makeSharedRef<Verbose<>>("one");
    auto b = makeSharedRef<Verbose<>>("two");
    swap(a, b);
    EXPECT_EQ("one", b.get().name());
    EXPECT_EQ("two", a.get().name());
}

TEST(SharedRef, container) {
    auto container = SharedContainer<Verbose<>>{};
    container.emplace_back(std::make_shared<Verbose<>>("one"));
    container.emplace_back(std::make_shared<Verbose<>>("two"));
    container.emplace_back(std::make_shared<Verbose<>>("three"));

    print(container);

    for (const auto& elem : container) {
        // explicitly get reference
        std::cout << elem.get().name() << std::endl;
    }

    // implicit conversion
    for (const Verbose<>& elem : container) {
        std::cout << elem.name() << std::endl;
    }

    // Copying a container of shared references
    // does not copy the referenced objects.
    auto container_copy = container;
    print(container_copy);

    // Moving a container of shared references
    // does not move the referenced objects.
    auto container_move = std::move(container);
    print(container_move);
}
