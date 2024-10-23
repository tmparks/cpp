#include "Verbose.hpp"
#include <gmock/gmock.h>
#include <experimental/propagate_const>
#include <iostream>
#include <memory>
#include <vector>

using namespace testing;
using namespace testing::internal;

template <typename T>
using SharedPtr = std::experimental::propagate_const<std::shared_ptr<T>>;

template <typename T>
using SharedVector = std::vector<SharedPtr<T>>;

template <typename T>
using UniquePtr = std::experimental::propagate_const<std::unique_ptr<T>>;

template <typename T>
using UniqueVector = std::vector<UniquePtr<T>>;

bool isConst(Verbose<>&) { return false; }

bool isConst(const Verbose<>&) { return true; }

TEST(ConstPointer, mutable_shared_ptr) {
    // const pointer to mutable object
    const auto p = std::make_shared<Verbose<>>("unqual");
    EXPECT_FALSE(isConst(*p));
}

TEST(ConstPointer, const_shared_ptr) {
    // const pointer to const object
    const auto p = SharedPtr<Verbose<>>{std::make_shared<Verbose<>>("qual")};
    EXPECT_TRUE(isConst(*p));
}

TEST(ConstPointer, const_shared_vector) {
    // const pointers to const objects
    auto v = SharedVector<Verbose<>>{};
    v.emplace_back(std::make_shared<Verbose<>>("one"));
    v.emplace_back(std::make_shared<Verbose<>>("two"));
    v.emplace_back(std::make_shared<Verbose<>>("three"));

    for (auto& p : v) {
        EXPECT_FALSE(isConst(*p));
    }

    for (const auto& p : v) {
        EXPECT_TRUE(isConst(*p));
    }
}

TEST(ConstPointer, mutable_unique_ptr) {
    // const pointer to mutable object
    const auto p = std::make_unique<Verbose<>>("unqual");
    EXPECT_FALSE(isConst(*p));
}

TEST(ConstPointer, const_unique_ptr) {
    // const pointer to const object
    const auto p = UniquePtr<Verbose<>>(std::make_unique<Verbose<>>("qual"));
    EXPECT_TRUE(isConst(*p));
}

TEST(ConstPointer, const_unique_vector) {
    // const pointers to const objects
    auto v = UniqueVector<Verbose<>>{};
    v.emplace_back(std::make_unique<Verbose<>>("one"));
    v.emplace_back(std::make_unique<Verbose<>>("two"));
    v.emplace_back(std::make_unique<Verbose<>>("three"));

    for (auto& p : v) {
        EXPECT_FALSE(isConst(*p));
    }

    for (const auto& p : v) {
        EXPECT_TRUE(isConst(*p));
    }
}
