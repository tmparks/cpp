#include "Verbose.hpp"
#include <iostream>
#include <utility>

Verbose::Verbose(std::string&& name) : name_(name)
{
    std::cout << name_ << ": constructor" << std::endl;
}

Verbose::Verbose(const Verbose& other) : name_(other.name_)
{
    std::cout << name_ << ": copy constructor" << std::endl;
}

Verbose::Verbose(Verbose&& other) : name_(other.name_)
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
        TestableVerbose() : TestableVerbose(gsl::czstring(__func__)) {}
        explicit TestableVerbose(std::string&& name) : Verbose(std::move(name)) {}
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
    auto v1 = TestableVerbose { };
    auto actual = GetCapturedStdout();
    EXPECT_THAT(actual, HasSubstr("constructor"));
    EXPECT_THAT(actual, Not(HasSubstr("copy")));
    EXPECT_THAT(actual, Not(HasSubstr("move")));
}

TEST(Verbose, CopyConstructor)
{
    auto v1 = TestableVerbose { };
    CaptureStdout();
    auto v2 { v1 };
    auto actual = GetCapturedStdout();
    EXPECT_THAT(actual, HasSubstr("constructor"));
    EXPECT_THAT(actual, HasSubstr("copy"));
    EXPECT_THAT(actual, Not(HasSubstr("move")));
}

TEST(Verbose, MoveConstructor)
{
    TestableVerbose v1 = TestableVerbose { };
    CaptureStdout();
    auto v2 { std::move(v1) };
    auto actual = GetCapturedStdout();
    EXPECT_THAT(actual, HasSubstr("constructor"));
    EXPECT_THAT(actual, Not(HasSubstr("copy")));
    EXPECT_THAT(actual, HasSubstr("move"));
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
}

TEST(Verbose, Destructor)
{
    {
        auto v1 = TestableVerbose { };
        CaptureStdout();
    }
    auto actual = GetCapturedStdout();
    EXPECT_THAT(actual, HasSubstr("destructor"));
}
