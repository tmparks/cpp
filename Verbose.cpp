#include "Verbose.hpp"
#include <iostream>

Verbose::Verbose(const std::string& name) : name_ { name }
{
    std::cout << name_ << ": constructor" << std::endl;
}

Verbose::Verbose(const Verbose& other) : name_ { other.name_ }
{
    std::cout << name_ << ": copy constructor" << std::endl;
}

Verbose::Verbose(Verbose&& other) : name_ { other.name_ }
{
    std::cout << name_ << ": move constructor" << std::endl;
}

Verbose& Verbose::operator=(const Verbose& other)
{
    std::cout << name_ << ": copy assignment" << std::endl;
    name_ = other.name_;
    return *this;
}

Verbose& Verbose::operator=(Verbose&& other)
{
    std::cout << name_ << ": move assignment" << std::endl;
    name_ = other.name_;
    return *this;
}

Verbose::~Verbose()
{
    std::cout << name_ << ": destructor" << std::endl;
}

////////////////////////////////////////////////////////////////////////////////

#include <gmock/gmock.h>
#include <gsl/gsl>

using namespace testing;
using namespace testing::internal;

namespace // Anonymous namespace for definitions that are local to this file.
{
    class TestableVerbose : private Verbose
    {
    public:
        TestableVerbose() : TestableVerbose { gsl::czstring(__func__) } {}
        explicit TestableVerbose(const std::string& name) : Verbose { name } {}
        TestableVerbose(const TestableVerbose&) = default;
        TestableVerbose(TestableVerbose&&) = default;
        TestableVerbose& operator=(const TestableVerbose&) = default;
        TestableVerbose& operator=(TestableVerbose&&) = default;
        ~TestableVerbose() override = default;
    };
} // anonymous namespace

TEST(Verbose, Constructor)
{
    CaptureStdout();
    auto v1 = TestableVerbose { "one" };
    auto v2 = TestableVerbose { "two" };
    auto actual = GetCapturedStdout();
    EXPECT_THAT(actual, HasSubstr("constructor"));
    EXPECT_THAT(actual, Not(HasSubstr("copy")));
    EXPECT_THAT(actual, Not(HasSubstr("move")));
    std::cout << std::endl << actual << std::endl;
}

TEST(Verbose, CopyConstructor)
{
    auto v1 = TestableVerbose { "one" };
    CaptureStdout();
    auto v2 = v1;
    auto actual = GetCapturedStdout();
    EXPECT_THAT(actual, HasSubstr("constructor"));
    EXPECT_THAT(actual, HasSubstr("copy"));
    EXPECT_THAT(actual, Not(HasSubstr("move")));
    std::cout << std::endl << actual << std::endl;
}

TEST(Verbose, MoveConstructor)
{
    auto v1 = TestableVerbose { "one" };
    CaptureStdout();
    auto v2 = std::move(v1);
    auto actual = GetCapturedStdout();
    EXPECT_THAT(actual, HasSubstr("constructor"));
    EXPECT_THAT(actual, Not(HasSubstr("copy")));
    EXPECT_THAT(actual, HasSubstr("move"));
    std::cout << std::endl << actual << std::endl;
}

TEST(Verbose, CopyAssignment)
{
    auto v1 = TestableVerbose { "one" };
    auto v2 = TestableVerbose { "two" };
    CaptureStdout();
    v1 = v2;
    auto actual = GetCapturedStdout();
    EXPECT_THAT(actual, HasSubstr("one"));
    EXPECT_THAT(actual, HasSubstr("assignment"));
    EXPECT_THAT(actual, HasSubstr("copy"));
    EXPECT_THAT(actual, Not(HasSubstr("move")));
    std::cout << std::endl << actual << std::endl;
}

TEST(Verbose, MoveAssignment)
{
    auto v1 = TestableVerbose { "one" };
    auto v2 = TestableVerbose { "two" };
    CaptureStdout();
    v1 = std::move(v2);
    auto actual = GetCapturedStdout();
    EXPECT_THAT(actual, HasSubstr("one"));
    EXPECT_THAT(actual, HasSubstr("assignment"));
    EXPECT_THAT(actual, Not(HasSubstr("copy")));
    EXPECT_THAT(actual, HasSubstr("move"));
    std::cout << std::endl << actual << std::endl;
}

TEST(Verbose, Destructor)
{
    {
        auto v1 = TestableVerbose { "one" };
        auto v2 = TestableVerbose { "two" };
        CaptureStdout();
    }
    auto actual = GetCapturedStdout();
    EXPECT_THAT(actual, HasSubstr("destructor"));
    std::cout << std::endl << actual << std::endl;
}

TEST(Verbose, Swap)
{
    auto v1 = TestableVerbose { "one" };
    auto v2 = TestableVerbose { "two" };

    CaptureStdout();
    std::swap(v1, v2); 
    auto actual = GetCapturedStdout();
    std::cout << std::endl << actual << std::endl;
}
