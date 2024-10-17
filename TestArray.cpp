#include "Array.hpp"
#include "Verbose.hpp"
#include <gmock/gmock.h>

using namespace testing;
using namespace testing::internal;

namespace { // anonymous namespace for definitions that are local to this file
    class TestableArray : public Verbose<Array> {
    public:
        using Verbose<Array>::Verbose;
        using Verbose<Array>::data_;
        using Verbose<Array>::size_;
    };
} // anonymous namespace

TEST(Array, DefaultConstructor) {
    auto a = TestableArray{"a"}; // constructor
    EXPECT_EQ(0, a.size());
}

TEST(Array, MoveConstructor) {
    constexpr auto size = gsl::index{3};
    auto a0 = TestableArray{"a0", size}; // constructor
    for (auto i = gsl::index{0}; i < size; i++) {
        a0[i] = gsl::narrow_cast<double>(2 * i); // even
    }

    CaptureStdout();
    auto a1 = std::move(a0); // move constructor
    auto actual = GetCapturedStdout();

    EXPECT_THAT(actual, HasSubstr("move constructor"));
    EXPECT_THAT(actual, Not(HasSubstr("copy")));
    EXPECT_THAT(actual, Not(HasSubstr("assignment")));
    std::cout << std::endl << actual << std::endl;

    const auto& const0 = a0;
    const auto& const1 = a1;
    EXPECT_EQ(0, const0.size_);
    EXPECT_EQ(nullptr, const0.data_);
    EXPECT_EQ(size, const1.size());
    for (auto i = gsl::index{0}; i < size; i++) {
        EXPECT_EQ(const1[i], 2 * i); // even
    }
}

TEST(Array, CopyAssignment) {
    constexpr auto size = gsl::index{3};
    auto a0 = TestableArray{"a0", size}; // constructor
    auto a1 = TestableArray{"a1", size}; // constructor
    for (auto i = gsl::index{0}; i < size; i++) {
        a0[i] = gsl::narrow_cast<double>(2 * i);     // even
        a1[i] = gsl::narrow_cast<double>(2 * i + 1); // odd
    }

    CaptureStdout();
    a0 = a1; // copy assignment
    auto actual = GetCapturedStdout();

    EXPECT_THAT(actual, HasSubstr("copy assignment"));
    EXPECT_THAT(actual, Not(HasSubstr("move")));
    EXPECT_THAT(actual, Not(HasSubstr("constructor")));
    std::cout << std::endl << actual << std::endl;

    const auto& const0 = a0;
    const auto& const1 = a1;
    EXPECT_NE(const0.data_, const1.data_); // pointers should differ
    EXPECT_EQ(const0.size(), const1.size());
    for (auto i = gsl::index{0}; i < size; i++) {
        EXPECT_EQ(const0[i], const1[i]);
    }
}

TEST(Array, MoveAssignment) {
    constexpr auto size = gsl::index{3};
    auto a0 = TestableArray{"a0", size}; // constructor
    auto a1 = TestableArray{"a1", size}; // constructor
    for (auto i = gsl::index{0}; i < size; i++) {
        a0[i] = gsl::narrow_cast<double>(2 * i);     // even
        a1[i] = gsl::narrow_cast<double>(2 * i + 1); // odd
    }

    CaptureStdout();
    a0 = std::move(a1); // move assignment
    auto actual = GetCapturedStdout();

    EXPECT_THAT(actual, HasSubstr("move assignment"));
    EXPECT_THAT(actual, Not(HasSubstr("copy")));
    EXPECT_THAT(actual, Not(HasSubstr("constructor")));
    std::cout << std::endl << actual << std::endl;

    const auto& const0 = a0;
    const auto& const1 = a1;
    for (auto i = gsl::index{0}; i < size; i++) {
        EXPECT_EQ(const0[i], 2 * i + 1); // odd
    }
    EXPECT_EQ(0, const1.size_);
    EXPECT_EQ(nullptr, const1.data_);
}
