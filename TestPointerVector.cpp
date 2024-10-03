#include "PointerVector.hpp"
#include "PointerMap.hpp"

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

TEST(SharedPointerVector, emplace_back) {
    // Place elements into container as pointers.
    auto container = SharedPointerVector<Verbose>{};
    auto& base = container.base;
    base.emplace_back(std::make_shared<Verbose>("one"));
    base.emplace_back(std::make_shared<Verbose>("two"));
    base.emplace_back(std::make_shared<Verbose>("three"));

    print(container);
}

TEST(SharedPointerVector, shallow_copy) {
    // Place elements into container as pointers.
    auto container = SharedPointerVector<Verbose>{};
    auto& base = container.base;
    base.emplace_back(std::make_shared<Verbose>("one"));
    base.emplace_back(std::make_shared<Verbose>("two"));
    base.emplace_back(std::make_shared<Verbose>("three"));

    // Copying a container of pointers does not copy the referenced objects.
    auto container_copy = container;
    print(container_copy);

    container_copy[0].name() = "alfa";
    container_copy[1].name() = "bravo";
    container_copy[2].name() = "charlie";

    EXPECT_EQ(container, container_copy); // shallow copy
}

TEST(UniquePointerVector, uncopyable) {
    auto container = UniquePointerVector<Verbose>{};
    auto& base = container.base;
    base.emplace_back(std::make_unique<Verbose>("one"));
    base.emplace_back(std::make_unique<Verbose>("two"));
    base.emplace_back(std::make_unique<Verbose>("three"));

    // auto container_copy = container; // compile time error
}

TEST(SharedPointerVector, erase_remove) {
    auto container = SharedPointerVector<Verbose>{};
    auto& base = container.base;
    base.emplace_back(std::make_shared<Verbose>("one"));
    base.emplace_back(std::make_shared<Verbose>("two"));
    base.emplace_back(std::make_shared<Verbose>("three"));

    // In order to avoid copying/moving values,
    // use the base vector of pointers for erase-remove.
    auto predicate = [](const std::shared_ptr<Verbose>& v) {
        return v->name() == "two";
    };
    base.erase(std::remove_if(base.begin(), base.end(), predicate), base.end());

    print(container);
}
