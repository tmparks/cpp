#include "UniqueRef.hpp"
#include "Verbose.hpp"
#include "compat/gsl14.hpp"

#include <gmock/gmock.h>

#include <type_traits>

using namespace testing;
using namespace testing::internal;

// Anonymous namespace for definitions that are local to this file.
namespace {

    template <typename T, int N>
    using UniqueArray = std::array<UniqueRef<T>,N>;

    // Create a unique reference to a newly constructed object of type T.
    template <typename T, typename... Args>
    UniqueRef<T> create(Args&&... args) {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    // Test that two objects are distinct (different addresses)
    bool distinct(const Verbose& a, const Verbose& b) { return &a != &b; }

    // Test that two objects are equal (same name)
    bool equal(const Verbose& a, const Verbose& b) {
        return a.name() == b.name();
    }

    // Print the elements of a container of unique references.
    template <typename T>
    void print(const T& container) {
        for (auto& elem : container) {
            std::cout << gsl::czstring(__func__) << ": "
                      << elem // resolves to operator<< for Verbose
                      << std::endl;
            const auto& name = elem.get().name();
            EXPECT_THAT(name, Not(HasSubstr("copy")));
            EXPECT_THAT(name, Not(HasSubstr("move")));
        }
    }
} // anonymous namespace

TEST(UniqueRef, constructor) {
    auto a = create<Verbose>("one");
    auto b = create<Verbose>("two");
    // auto c = UniqueRef<Verbose> { }; // no dangling references!

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

TEST(UniqueRef, container) {
    UniqueArray<Verbose, 3> a1 {
        std::make_unique<Verbose>("one"),
        std::make_unique<Verbose>("two"),
        std::make_unique<Verbose>("three") };

    print(a1);

    for (size_t i = 0; i < a1.size(); i++) {
        const auto& elem = a1[i].get(); // explicitly call get()
        std::cout << elem.name() << std::endl;
    }

    for (size_t i = 0; i < a1.size(); i++) {
        const Verbose& elem = a1[i]; // implicit conversion (cannot use auto)
        std::cout << elem.name() << std::endl;
    }
}
