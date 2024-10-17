#include "Verbose.hpp"
#include "compat/gsl14.hpp"
#include <gmock/gmock.h>

using namespace testing;
using namespace testing::internal;

TEST(Verbose, Constructor) {
    CaptureStdout();
    auto v1 = Verbose<>{"one"};
    auto v2 = Verbose<>{"two"};
    auto actual = GetCapturedStdout();
    EXPECT_THAT(actual, HasSubstr("constructor"));
    EXPECT_THAT(actual, Not(HasSubstr("copy")));
    EXPECT_THAT(actual, Not(HasSubstr("move")));
    std::cout << std::endl << actual << std::endl;
}

TEST(Verbose, CopyConstructor) {
    auto v1 = Verbose<>{"one"};
    CaptureStdout();
    auto v2 = v1;
    auto actual = GetCapturedStdout();
    EXPECT_THAT(actual, HasSubstr("constructor"));
    EXPECT_THAT(actual, HasSubstr("copy"));
    EXPECT_THAT(actual, Not(HasSubstr("move")));
    std::cout << std::endl << actual << std::endl;
}

TEST(Verbose, MoveConstructor) {
    auto v1 = Verbose<>{"one"};
    CaptureStdout();
    auto v2 = std::move(v1);
    auto actual = GetCapturedStdout();
    EXPECT_THAT(actual, HasSubstr("constructor"));
    EXPECT_THAT(actual, Not(HasSubstr("copy")));
    EXPECT_THAT(actual, HasSubstr("move"));
    std::cout << std::endl << actual << std::endl;
}

TEST(Verbose, CopyAssignment) {
    auto v1 = Verbose<>{"one"};
    auto v2 = Verbose<>{"two"};
    CaptureStdout();
    v1 = v2;
    auto actual = GetCapturedStdout();
    EXPECT_THAT(actual, HasSubstr("one"));
    EXPECT_THAT(actual, HasSubstr("assignment"));
    EXPECT_THAT(actual, HasSubstr("copy"));
    EXPECT_THAT(actual, Not(HasSubstr("move")));
    std::cout << std::endl << actual << std::endl;
}

TEST(Verbose, MoveAssignment) {
    auto v1 = Verbose<>{"one"};
    auto v2 = Verbose<>{"two"};
    CaptureStdout();
    v1 = std::move(v2);
    auto actual = GetCapturedStdout();
    EXPECT_THAT(actual, HasSubstr("one"));
    EXPECT_THAT(actual, HasSubstr("assignment"));
    EXPECT_THAT(actual, Not(HasSubstr("copy")));
    EXPECT_THAT(actual, HasSubstr("move"));
    std::cout << std::endl << actual << std::endl;
}

TEST(Verbose, Destructor) {
    {
        auto v1 = Verbose<>{"one"};
        auto v2 = Verbose<>{"two"};
        CaptureStdout();
    }
    auto actual = GetCapturedStdout();
    EXPECT_THAT(actual, HasSubstr("destructor"));
    std::cout << std::endl << actual << std::endl;
}

TEST(Verbose, Swap) {
    auto v1 = Verbose<>{"one"};
    auto v2 = Verbose<>{"two"};

    CaptureStdout();
    std::swap(v1, v2);
    auto actual = GetCapturedStdout();
    EXPECT_THAT(actual, HasSubstr("one"));
    EXPECT_THAT(actual, HasSubstr("assignment"));
    EXPECT_THAT(actual, Not(HasSubstr("copy")));
    EXPECT_THAT(actual, HasSubstr("move"));
    std::cout << std::endl << actual << std::endl;
}

// Move constructor (not copy constructor) should be used when a
// vector reallocates due to growing to exceed its capacity
TEST(Verbose, VectorMove) {
    std::vector<Verbose<>> v{};
    v.reserve(1);
    auto limit = gsl::narrow_cast<gsl::index>(v.capacity());
    for (auto i = gsl::index{0}; i < limit; i++) {
        v.emplace_back(std::to_string(i));
    }
    CaptureStdout();
    for (auto i = limit; i < 2 * limit; i++) {
        v.emplace_back(std::to_string(i));
    }
    auto actual = GetCapturedStdout();
    EXPECT_THAT(actual, HasSubstr("0"));
    EXPECT_THAT(actual, HasSubstr("constructor"));
    EXPECT_THAT(actual, Not(HasSubstr("copy")));
    EXPECT_THAT(actual, HasSubstr("move"));
    std::cout << std::endl << actual << std::endl;
}
