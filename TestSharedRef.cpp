#include "SharedRef.hpp"
#include "Verbose.hpp"
#include "compat/gsl14.hpp"

#include <gmock/gmock.h>

using namespace testing;
using namespace testing::internal;

template <typename T>
using SharedVector = std::vector<SharedRef<T>>;

// Create a shared reference to a newly constructed object of type T.
template <typename T, typename... Args>
SharedRef<T> create(Args&&... args) {
    return SharedRef<T> { std::make_shared<T>(std::forward<Args>(args)...) };
}

// Test that two objects are distinct (different addresses)
bool distinct(const Verbose& a, const Verbose& b) { return &a != &b; }

// Test that two objects are equal (same name)
bool equal(const Verbose& a, const Verbose& b) { return a.name() == b.name(); }

// Print the elements of a vector of shared references.
void print(const SharedVector<Verbose>& v) {
    for (auto& elem : v) {
        std::cout << gsl::czstring(__func__) << ": "
                  << elem // resolves to operator<< for Verbose
                  << std::endl;
        const auto& name = elem.getRef().name();
        EXPECT_THAT(name, Not(HasSubstr("copy")));
        EXPECT_THAT(name, Not(HasSubstr("move")));
    }
}

TEST(SharedRef, constructor) {
    auto a = create<Verbose>("one");
    auto b = create<Verbose>("two");
    // auto c = SharedRef<Verbose> { }; // no dangling references!

    EXPECT_FALSE(equal(a, b));
    EXPECT_TRUE(distinct(a, b));
}

TEST(SharedRef, copy) {
    auto a = create<Verbose>("one");
    auto b = create<Verbose>("two");
    auto c = create<Verbose>("three");
    auto d = SharedRef<Verbose> { a }; // copy constructor
    c = b;                             // copy assignment

    // Copying modifies the reference, not the referenced object.

    EXPECT_TRUE(equal(a, d));
    EXPECT_FALSE(distinct(a, d)); // both refer to same object

    EXPECT_TRUE(equal(b, c));
    EXPECT_FALSE(distinct(b, c)); // both refer to same object

    const auto& cName = c.getRef().name();
    EXPECT_THAT(cName, EndsWith("two"));
    EXPECT_THAT(cName, Not(HasSubstr("three")));
    EXPECT_THAT(cName, Not(HasSubstr("copy")));
    EXPECT_THAT(cName, Not(HasSubstr("move")));
    EXPECT_THAT(cName, Not(HasSubstr("construct")));
    EXPECT_THAT(cName, Not(HasSubstr("assign")));

    const auto& dName = d.getRef().name();
    EXPECT_THAT(dName, EndsWith("one"));
    EXPECT_THAT(dName, Not(HasSubstr("copy")));
    EXPECT_THAT(dName, Not(HasSubstr("move")));
    EXPECT_THAT(dName, Not(HasSubstr("construct")));
    EXPECT_THAT(dName, Not(HasSubstr("assign")));
}

TEST(SharedRef, move) {
    auto a = create<Verbose>("one");
    auto b = create<Verbose>("two");
    auto c = create<Verbose>("three");
    auto d = SharedRef<Verbose> { std::move(a) }; // move constructor
    c = std::move(b);                             // move assignment

    // Moving modifies the reference, not the referenced object.

    const auto& cName = c.getRef().name();
    EXPECT_THAT(cName, EndsWith("two"));
    EXPECT_THAT(cName, Not(HasSubstr("three")));
    EXPECT_THAT(cName, Not(HasSubstr("copy")));
    EXPECT_THAT(cName, Not(HasSubstr("move")));
    EXPECT_THAT(cName, Not(HasSubstr("construct")));
    EXPECT_THAT(cName, Not(HasSubstr("assign")));

    const auto& dName = d.getRef().name();
    EXPECT_THAT(dName, EndsWith("one"));
    EXPECT_THAT(dName, Not(HasSubstr("copy")));
    EXPECT_THAT(dName, Not(HasSubstr("move")));
    EXPECT_THAT(dName, Not(HasSubstr("construct")));
    EXPECT_THAT(dName, Not(HasSubstr("assign")));
}

TEST(SharedRef, vector) {
    SharedVector<Verbose> v1;
    v1.emplace_back(create<Verbose>("one"));
    v1.emplace_back(create<Verbose>("two"));
    v1.emplace_back(create<Verbose>("three"));

    print(v1);

    for (size_t i = 0; i < v1.size(); i++) {
        const auto& elem = v1[i].getRef(); // explicitly call getRef()
        std::cout << elem.name() << std::endl;
    }

    for (size_t i = 0; i < v1.size(); i++) {
        const Verbose& elem = v1[i]; // implicit conversion (cannot use auto)
        std::cout << elem.name() << std::endl;
    }

    // Copying a vector of shared references does not copy the referenced objects.
    auto v2 = v1;
    print(v2);
}
