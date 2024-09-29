#include "PointerVector.hpp"

#include "Verbose.hpp"
#include "compat/gsl14.hpp"

#include <gmock/gmock.h>

using namespace testing;
using namespace testing::internal;

// Anonymous namespace for definitions that are local to this file.
namespace {
    // Print the elements of a PointerVector.
    void print(const SharedPointerVector<Verbose>& container) {
        for (const auto& elem : container) {
            std::cout << gsl::czstring{__func__} << ": " << elem << std::endl;
            const auto& name = elem.name();
            EXPECT_THAT(name, Not(HasSubstr("copy")));
            EXPECT_THAT(name, Not(HasSubstr("move")));
        }
    }
} // anonymous namespace

TEST(SharedPointerVector, container) {
    // Place elements into container as pointers.
    auto container = SharedPointerVector<Verbose>{};
    container.emplace_back(std::make_shared<Verbose>("one"));
    container.emplace_back(std::make_shared<Verbose>("two"));
    container.emplace_back(std::make_shared<Verbose>("three"));

    print(container);

    // Access elements of the container as references.
    for (const auto& elem : container) {
        std::cout << elem.name() << std::endl;
    }

    // Copying a container of pointers does not copy the referenced objects.
    auto container_copy = container;
    print(container_copy);
}
