#include "SharedRef.hpp"
#include "Verbose.hpp"
#include "compat/gsl14.hpp"

#include <gtest/gtest.h>

template <typename T>
using SharedVector = std::vector<SharedRef<T>>;

bool distinct(const Verbose& a, const Verbose& b) {
    return &a != &b; // distinct objects
}

bool equal(const Verbose& a, const Verbose& b) {
    return a.name() == b.name(); // same name
}

void print(const SharedVector<Verbose>& v) {
    for (auto& elem : v) {
        std::cout << gsl::czstring(__func__) << ": " << elem << std::endl;
    }
}

TEST(SharedRef, constructor) {
    auto a = SharedRef<Verbose> { "one" };
    auto b = SharedRef<Verbose> { "two" };
    EXPECT_FALSE(equal(a, b));
    EXPECT_TRUE(distinct(a, b));
}

TEST(SharedRef, copy) {
    auto a = SharedRef<Verbose> { "one" };
    auto b = SharedRef<Verbose> { a };
    EXPECT_TRUE(distinct(a, b));
}

TEST(SharedRef, move) {
    auto a = SharedRef<Verbose> { "one" };
    auto b = SharedRef<Verbose> { std::move(a) };
    EXPECT_EQ(nullptr, a.getPtr()); // has been moved from
    EXPECT_EQ("one", b.getRef().name()); // SharedRef has been moved, Verbose has not
}

TEST(SharedRef, vector) {
    SharedVector<Verbose> v;
    v.emplace_back("one");
    v.emplace_back("two");
    v.emplace_back("three");

    print(v);

    for (size_t i = 0; i < v.size(); i++) {
        const auto& elem = v[i].getRef(); // explicitly call getRef()
        std::cout << elem.name() << std::endl;
    }

    for (size_t i = 0; i < v.size(); i++) {
        const Verbose& elem = v[i]; // implicit conversion (cannot use auto)
        std::cout << elem.name() << std::endl;
    }
}
