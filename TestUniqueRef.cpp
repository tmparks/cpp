#include "UniqueRef.hpp"
#include "Verbose.hpp"
#include "compat/gsl14.hpp"

#include <gmock/gmock.h>
#include <vector>

using namespace testing;
using namespace testing::internal;

// Anonymous namespace for definitions that are local to this file.
namespace {
    template <typename T>
    using UniqueContainer = std::vector<UniqueRef<T>>;

    // Test that two objects are distinct (different addresses)
    bool distinct(const Verbose<>& a, const Verbose<>& b) { return &a != &b; }

    // Test that two objects are equal (same name)
    bool equal(const Verbose<>& a, const Verbose<>& b) {
        return a.name() == b.name();
    }

    // Print the elements of a container of smart references.
    void print(const UniqueContainer<Verbose<>>& container) {
        for (const Verbose<>& elem : container) {
            std::cout << gsl::czstring{__func__} << ": " << elem << std::endl;
            const auto& name = elem.name();
            EXPECT_THAT(name, Not(HasSubstr("copy")));
            EXPECT_THAT(name, Not(HasSubstr("move")));
        }
    }
} // anonymous namespace

TEST(UniqueRef, constructor) {
    auto a = makeUniqueRef<Verbose<>>("one");
    auto b = makeUniqueRef<Verbose<>>("two");
    // auto c = UniqueRef<Verbose<>> { }; // no dangling references!

    EXPECT_FALSE(equal(a, b));
    EXPECT_TRUE(distinct(a, b));
}

TEST(UniqueRef, copy) {
    // UniqueRef is not copyable because unique_ptr is not copyable.
    EXPECT_FALSE(std::is_copy_constructible<UniqueRef<int>>::value);
    EXPECT_FALSE(std::is_copy_assignable<UniqueRef<int>>::value);
}

TEST(UniqueRef, move) {
    // UniqueRef is moveable because unique_ptr is moveable.
    EXPECT_TRUE(std::is_move_constructible<UniqueRef<int>>::value);
    EXPECT_TRUE(std::is_move_assignable<UniqueRef<int>>::value);

    auto a = makeUniqueRef<Verbose<>>("one");
    auto b = makeUniqueRef<Verbose<>>("two");
    auto c = makeUniqueRef<Verbose<>>("three");
    auto d = UniqueRef<Verbose<>>{std::move(a)}; // move constructor

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

TEST(UniqueRef, reset) {
    auto a = makeUniqueRef<Verbose<>>("one");
    a.reset(std::make_unique<Verbose<>>("two"));
    EXPECT_EQ("two", a.get().name());
}

TEST(UniqueRef, swap) {
    auto a = makeUniqueRef<Verbose<>>("one");
    auto b = makeUniqueRef<Verbose<>>("two");
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

    // implicit conversion
    for (const Verbose<>& elem : container) {
        std::cout << elem.name() << std::endl;
    }

    // Moving a container of unique references
    // does not move the references objects.
    auto container_move = std::move(container);
    print(container_move);
}
