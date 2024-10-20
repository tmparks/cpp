#include "Shape.hpp"
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

    // Both refer to the same object (same addresses)
    template <typename T>
    bool same(const UniqueRef<T>& a, const UniqueRef<T>& b) {
        return &a.get() == &b.get();
    }

    const std::string& name(const Verbose<Circle>& v) { return v.name(); }

    const std::string& name(const Verbose<>& v) { return v.name(); }

    double area(const Shape& a) { return a.area(); }

    // Print the elements of a container of smart references.
    template <typename T>
    void print(
            const UniqueContainer<Verbose<T>>& container,
            const std::string& prefix = "print: ") {
        for (const auto& elem : container) {
            std::cout << prefix << name(elem) << std::endl;
            EXPECT_THAT(name(elem), Not(HasSubstr("copy")));
            EXPECT_THAT(name(elem), Not(HasSubstr("move")));
        }
    }
} // anonymous namespace

TEST(UniqueRef, constructor) {
    auto a = makeUniqueRef<Verbose<Circle>>("one", 1.0);
    auto b = makeUniqueRef<Verbose<Circle>>("two", 2.0);
    // auto c = UniqueRef<Verbose<Circle>> { }; // no dangling references!

    EXPECT_FALSE(same(a, b));
    EXPECT_NE(name(a), name(b));
    EXPECT_NE(area(a), area(b));
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

    auto a = makeUniqueRef<Verbose<Circle>>("one", 1.0);
    auto b = makeUniqueRef<Verbose<Circle>>("two", 2.0);
    auto c = makeUniqueRef<Verbose<Circle>>("three", 3.0);
    auto d = UniqueRef<Verbose<Circle>>{std::move(a)}; // move constructor

    c = std::move(b); // move assignment

    // Moving modifies the reference, not the object.

    EXPECT_THAT(name(c), EndsWith("two"));
    EXPECT_THAT(name(c), Not(HasSubstr("three")));
    EXPECT_THAT(name(c), Not(HasSubstr("copy")));
    EXPECT_THAT(name(c), Not(HasSubstr("move")));
    EXPECT_THAT(name(c), Not(HasSubstr("construct")));
    EXPECT_THAT(name(c), Not(HasSubstr("assign")));

    EXPECT_THAT(name(d), EndsWith("one"));
    EXPECT_THAT(name(d), Not(HasSubstr("copy")));
    EXPECT_THAT(name(d), Not(HasSubstr("move")));
    EXPECT_THAT(name(d), Not(HasSubstr("construct")));
    EXPECT_THAT(name(d), Not(HasSubstr("assign")));
}

TEST(UniqueRef, rebind) {
    CaptureStdout();
    auto a = UniqueRef<Shape>(new Verbose<Square>{"square", 3.0});
    auto b = std::move(a); // move reference, not object
    b = UniqueRef<Shape>{new Verbose<Circle>{"circle", 3.0}};
    auto actual = GetCapturedStdout();
    EXPECT_NE(9.0, area(b));
    EXPECT_THAT(actual, Not(HasSubstr("copy")));
    EXPECT_THAT(actual, Not(HasSubstr("move")));
    std::cout << std::endl << actual << std::endl;
}

TEST(UniqueRef, swap) {
    auto a = makeUniqueRef<Verbose<>>("one");
    auto b = makeUniqueRef<Verbose<>>("two");
    swap(a, b);
    EXPECT_EQ("one", name(b));
    EXPECT_EQ("two", name(a));
}

TEST(UniqueRef, container) {
    auto container = UniqueContainer<Verbose<>>{};
    container.emplace_back(std::make_unique<Verbose<>>("one"));
    container.emplace_back(std::make_unique<Verbose<>>("two"));
    container.emplace_back(std::make_unique<Verbose<>>("three"));

    print(container, "original: ");
    EXPECT_EQ("two", name(container[1]));

    // Moving a container of references
    // does not move the objects.
    auto container_move = std::move(container);
    print(container_move, "move: ");
}
