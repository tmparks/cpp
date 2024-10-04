#include "PointerVector.hpp"

#include "Verbose.hpp"
#include "compat/gsl14.hpp"

#include <gmock/gmock.h>

using namespace testing;
using namespace testing::internal;

// Anonymous namespace for definitions that are local to this file.
namespace {
    // Print the elements of a PointerVector.
    void print(const RawPointerVector<Verbose>& container) {
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

    print(reference(container));
}

TEST(SharedPointerVector, push_back) {
    // Place elements into container as pointers.
    auto container = SharedPointerVector<Verbose>{};
    auto& base = container.base;
    base.push_back(std::make_shared<Verbose>("one"));
    base.push_back(std::make_shared<Verbose>("two"));
    base.push_back(std::make_shared<Verbose>("three"));

    print(reference(container));
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
    print(reference(container_copy));

    container_copy[0].name() = "alfa";
    container_copy[1].name() = "bravo";
    container_copy[2].name() = "charlie";

    EXPECT_EQ(container, container_copy); // shallow copy
    EXPECT_NE("one", container[0].name());
    EXPECT_NE("two", container[1].name());
}

TEST(SharedPointerVector, front_back) {
    // Place elements into container as pointers.
    auto container = SharedPointerVector<Verbose>{};
    auto& base = container.base;
    base.emplace_back(std::make_shared<Verbose>("front"));
    base.emplace_back(std::make_shared<Verbose>("middle"));
    base.emplace_back(std::make_shared<Verbose>("back"));

    EXPECT_EQ("front", container.front().name());
    EXPECT_EQ("back", container.back().name());
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

    print(reference(container));
}

TEST(SharedPointerVector, mask) {
    auto container = SharedPointerVector<Verbose>{};
    auto& base = container.base;
    base.emplace_back(std::make_shared<Verbose>("one"));
    base.emplace_back(std::make_shared<Verbose>("two"));
    base.emplace_back(std::make_shared<Verbose>("three"));

    auto masked = mask(container, std::vector<bool>{true, false, true});

    masked[1].name() = "tres";
    EXPECT_EQ("tres", container[2].name());

    EXPECT_EQ(container.size() - 1, masked.size());
    EXPECT_EQ(container[0].name(), masked[0].name());
    EXPECT_EQ(container[2].name(), masked[1].name());

    print(reference(container));
}

TEST(UniquePointerVector, uncopyable) {
    auto container = UniquePointerVector<Verbose>{};
    auto& base = container.base;
    base.emplace_back(std::make_unique<Verbose>("one"));
    base.emplace_back(std::make_unique<Verbose>("two"));
    base.emplace_back(std::make_unique<Verbose>("three"));

    // auto container_copy = container; // compile time error
}
