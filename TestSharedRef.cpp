#include "SharedRef.hpp"

#include "Shape.hpp"
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

    // Both refer to the same object (same address)
    template <typename T>
    bool same(const SharedRef<T>& a, const SharedRef<T>& b) {
        return &a.get() == &b.get();
    }

    bool less(const Verbose<>& left, const Verbose<>& right) {
        return left.name() < right.name();
    }

    const std::string& name(const Verbose<Circle>& v) { return v.name(); }

    const std::string& name(const Verbose<>& v) { return v.name(); }

    double area(const Shape& a) { return a.area(); }

    // Print the elements of a container of smart references.
    template <typename T>
    void print(
            const SharedContainer<Verbose<T>>& container,
            const std::string& prefix = "print: ") {
        for (const auto& elem : container) {
            std::cout << prefix << name(elem) << std::endl;
            EXPECT_THAT(name(elem), Not(HasSubstr("copy")));
            EXPECT_THAT(name(elem), Not(HasSubstr("move")));
        }
    }
} // anonymous namespace

// NOLINTBEGIN(*-avoid-magic-numbers)

TEST(SharedRef, constructor) {
    auto a = makeSharedRef<Verbose<Circle>>("one", 1.0);
    auto b = makeSharedRef<Verbose<Circle>>("two", 2.0);
    // auto c = SharedRef<Verbose<Circle>> { }; // no dangling references!

    EXPECT_FALSE(same(a, b));
    EXPECT_NE(name(a), name(b));
    EXPECT_NE(area(a), area(b));
}

TEST(SharedRef, copy) {
    // SharedRef is copyable because shared_ptr is copyable.
    EXPECT_TRUE(std::is_copy_constructible<SharedRef<Shape>>::value);
    EXPECT_TRUE(std::is_copy_assignable<SharedRef<Shape>>::value);

    auto a = makeSharedRef<Verbose<Circle>>("one", 1.0);
    auto b = makeSharedRef<Verbose<Circle>>("two", 2.0);
    auto c = makeSharedRef<Verbose<Circle>>("three", 3.0);
    auto d = SharedRef<Verbose<Circle>>{a}; // copy constructor

    c = b; // copy assignment

    // Copying modifies the reference, not the object.

    EXPECT_TRUE(same(a, d)); // both refer to same object
    EXPECT_EQ(name(a), name(d));
    EXPECT_EQ(area(a), area(d));

    EXPECT_TRUE(same(b, c)); // both refer to same object
    EXPECT_EQ(name(b), name(c));
    EXPECT_EQ(area(b), area(c));

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

TEST(SharedRef, move) {
    // SharedRef is moveable because shared_ptr is moveable.
    EXPECT_TRUE(std::is_move_constructible<SharedRef<Shape>>::value);
    EXPECT_TRUE(std::is_move_assignable<SharedRef<Shape>>::value);

    auto a = makeSharedRef<Verbose<Circle>>("one", 1.0);
    auto b = makeSharedRef<Verbose<Circle>>("two", 2.0);
    auto c = makeSharedRef<Verbose<Circle>>("three", 3.0);
    auto d = SharedRef<Verbose<Circle>>{std::move(a)}; // move constructor

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

TEST(SharedRef, rebind) {
    CaptureStdout();
    auto a = SharedRef<Shape>{new Verbose<Square>{"square", 3.0}};
    auto b = a; // copy reference, not object
    b = SharedRef<Shape>{new Verbose<Circle>{"circle", 3.0}};
    auto actual = GetCapturedStdout();
    EXPECT_NE(area(a), area(b));
    EXPECT_THAT(actual, Not(HasSubstr("copy")));
    EXPECT_THAT(actual, Not(HasSubstr("move")));
    std::cout << std::endl << actual << std::endl;
}

TEST(SharedRef, swap) {
    auto a = SharedRef<Shape>{new Verbose<Square>{"square", 1.0}};
    auto b = SharedRef<Shape>{new Verbose<Circle>{"circle", 2.0}};
    CaptureStdout();
    swap(a, b); // swap references, not objects
    auto actual = GetCapturedStdout();
    EXPECT_EQ(1.0, area(b));
    EXPECT_TRUE(actual.empty()) << actual;
}

TEST(SharedRef, container) {
    auto container = SharedContainer<Verbose<>>{};
    container.emplace_back(std::make_shared<Verbose<>>("one"));
    container.emplace_back(std::make_shared<Verbose<>>("two"));
    container.emplace_back(std::make_shared<Verbose<>>("three"));

    print(container, "original :");
    EXPECT_EQ("two", name(container[1]));

    // Copying a container of references
    // does not copy the referenced objects.
    auto container_copy = container;
    print(container_copy, "copy: ");

    // Sorting a container of references
    // does not affect the original container.
#if __cplusplus >= 202002L // since C++20
    std::ranges::sort(container_copy, less);
#else  // until C++20
    std::sort(container_copy.begin(), container_copy.end(), less);
#endif // C++20
    print(container_copy, "sorted: ");
    EXPECT_EQ(name(container[2]), name(container_copy[1]));

    // Moving a container of references
    // does not move the objects.
    auto container_move = std::move(container);
    print(container_move, "move: ");
}

// NOLINTEND(*-avoid-magic-numbers)
