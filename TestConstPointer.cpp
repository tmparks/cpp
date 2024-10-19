#include "CoQual.hpp"
#include "Verbose.hpp"
#include <gmock/gmock.h>
#include <iostream>
#include <memory>
#include <vector>

using namespace testing;
using namespace testing::internal;

template <typename T>
using SharedPtr = CoQual<std::shared_ptr<T>>;

template <typename T>
using UniquePtr = CoQual<std::unique_ptr<T>>;

void check(Verbose<>& v) {
    std::cout << __func__ << ": " << v.name() << " is mutable" << std::endl;
}

void check(const Verbose<>& v) {
    std::cout << __func__ << ": " << v.name() << " is const" << std::endl;
}

TEST(Qual, unqual_shared_ptr) {
    const auto p = std::make_shared<Verbose<>>("unqual");
    CaptureStdout();
    check(*p);
    auto actual = GetCapturedStdout();
    EXPECT_THAT(actual, HasSubstr("mutable"));
    EXPECT_THAT(actual, Not(HasSubstr("const")));
    std::cout << std::endl << actual << std::endl;
}

TEST(Qual, qual_shared_ptr) {
    const auto p = SharedPtr<Verbose<>>{std::make_shared<Verbose<>>("unqual")};
    CaptureStdout();
    check(*p);
    auto actual = GetCapturedStdout();
    EXPECT_THAT(actual, HasSubstr("const"));
    EXPECT_THAT(actual, Not(HasSubstr("mutable")));
    std::cout << std::endl << actual << std::endl;
}

TEST(Qual, unqual_unique_ptr) {
    const auto p = std::make_unique<Verbose<>>("unqual");
    CaptureStdout();
    check(*p);
    auto actual = GetCapturedStdout();
    EXPECT_THAT(actual, HasSubstr("mutable"));
    EXPECT_THAT(actual, Not(HasSubstr("const")));
    std::cout << std::endl << actual << std::endl;
}

TEST(Qual, qual_unique_ptr) {
    const auto p = UniquePtr<Verbose<>>(std::make_unique<Verbose<>>("unqual"));
    CaptureStdout();
    check(*p);
    auto actual = GetCapturedStdout();
    EXPECT_THAT(actual, HasSubstr("const"));
    EXPECT_THAT(actual, Not(HasSubstr("mutable")));
    std::cout << std::endl << actual << std::endl;
}