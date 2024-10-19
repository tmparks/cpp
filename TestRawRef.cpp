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

    // Print the elements of a container of smart references.
    void print(
            const RefContainer<Verbose<>>& container,
            std::string prefix = "print: ") {
        for (const Verbose<>& elem : container) {
            std::cout << prefix << elem << std::endl;
            const auto& name = elem.name();
            EXPECT_THAT(name, Not(HasSubstr("copy")));
            EXPECT_THAT(name, Not(HasSubstr("move")));
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
    EXPECT_EQ("two", refContainer[1].get().name()) << "get";
    EXPECT_EQ("two", static_cast<Verbose<>&>(refContainer[1]).name())
            << "explicit";
    const Verbose<>& two = refContainer[1];
    EXPECT_EQ("two", two.name()) << "implicit";

    // Copying a container of shared references
    // does not copy or move the referenced objects.
    auto container_copy = refContainer;
    print(container_copy, "copy: ");

    // Sorting a container of shared references
    // does not affect the original container.
#if __cplusplus >= 202002L // since C++20
    std::ranges::sort(refContainer, less);
#else  // until C++20
    std::sort(refContainer.begin(), refContainer.end(), less);
#endif // C++20

    print(refContainer, "sorted: ");
    for (const auto& elem : container) {
        std::cout << "original: " << elem << std::endl;
    }
    EXPECT_EQ(container[2].name(), refContainer[1].get().name());
}
