#include "Shape.hpp"
#include "Verbose.hpp"
#include "compat/gsl14.hpp"
#include <functional>
#include <gmock/gmock.h>
#include <vector>

using namespace testing;
using namespace testing::internal;

// Anonymous namespace for definitions that are local to this file.
namespace {
    template <typename T>
    using RefContainer = std::vector<std::reference_wrapper<T>>;

    bool less(const Verbose<>& left, const Verbose<>& right) {
        return left.name() < right.name();
    }

    const std::string& name(const Verbose<>& v) { return v.name(); }

    // Print the elements of a container of smart references.
    template <typename T>
    void print(
            const RefContainer<Verbose<T>>& container,
            const std::string& prefix = "print: ") {
        for (const auto& elem : container) {
            std::cout << prefix << name(elem) << std::endl;
            EXPECT_THAT(name(elem), Not(HasSubstr("copy")));
            EXPECT_THAT(name(elem), Not(HasSubstr("move")));
        }
    }
} // anonymous namespace

TEST(RawRef, container) {
    auto container = std::vector<Verbose<>>{};
    container.reserve(3); // avoid reallocation
    container.emplace_back("one");
    container.emplace_back("two");
    container.emplace_back("three");

    auto refContainer =
            RefContainer<Verbose<>>(container.begin(), container.end());

    print(refContainer, "ref: ");
    EXPECT_EQ("two", name(refContainer[1]));

    // Copying a container of references
    // does not copy or move the referenced objects.
    auto container_copy = refContainer;
    print(container_copy, "copy: ");

    // Sorting a container of references
    // does not affect the original container.
#if __cplusplus >= 202002L // since C++20
    std::ranges::sort(refContainer, less);
#else  // until C++20
    std::sort(refContainer.begin(), refContainer.end(), less);
#endif // C++20

    print(refContainer, "sorted: ");
    EXPECT_EQ(name(container[2]), name(refContainer[1]));
}
