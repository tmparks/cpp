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

Verbose::Verbose(Verbose&& other) noexcept : name_ { std::move(other.name_) }
{
    std::cout << name_ << ": move constructor" << std::endl;
}

Verbose& Verbose::operator=(const Verbose& other)
{
    std::cout << name_ << ": copy assignment" << std::endl;
    name_ = other.name_;
    return *this;
}

Verbose& Verbose::operator=(Verbose&& other) noexcept
{
    std::cout << name_ << ": move assignment" << std::endl;
    name_ = std::move(other.name_);
    return *this;
}

Verbose::~Verbose()
{
    std::cout << name_ << ": destructor" << std::endl;
}

void swap(Verbose& left, Verbose& right) noexcept
{
    std::cout << left.name_ << ": swap" << std::endl;
    using std::swap; // enable Argument Dependent Lookup
    swap(left.name_, right.name_);
}

////////////////////////////////////////////////////////////////////////////////

#include <gmock/gmock.h>
#include <gsl/gsl>

using namespace testing;
using namespace testing::internal;

TEST(Verbose, Constructor)
{
    CaptureStdout();
    auto v1 = Verbose { "one" };
    auto v2 = Verbose { "two" };
    auto actual = GetCapturedStdout();
    EXPECT_THAT(actual, HasSubstr("constructor"));
    EXPECT_THAT(actual, Not(HasSubstr("copy")));
    EXPECT_THAT(actual, Not(HasSubstr("move")));
    std::cout << std::endl << actual << std::endl;
}

TEST(Verbose, CopyConstructor)
{
    auto v1 = Verbose { "one" };
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
    auto v1 = Verbose { "one" };
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
    auto v1 = Verbose { "one" };
    auto v2 = Verbose { "two" };
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
    auto v1 = Verbose { "one" };
    auto v2 = Verbose { "two" };
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
        auto v1 = Verbose { "one" };
        auto v2 = Verbose { "two" };
        CaptureStdout();
    }
    auto actual = GetCapturedStdout();
    EXPECT_THAT(actual, HasSubstr("destructor"));
    std::cout << std::endl << actual << std::endl;
}

TEST(Verbose, Swap)
{
    auto v1 = Verbose { "one" };
    auto v2 = Verbose { "two" };

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
TEST(Verbose, VectorMove)
{
    std::vector<Verbose> v { };
    v.reserve(1);
    auto limit = gsl::index(v.capacity());
    for ( auto i = gsl::index(0); i < limit; i++)
    {
        v.emplace_back(std::to_string(i));
    }
    CaptureStdout();
    for (auto i = limit; i < 2*limit; i++)
    {
        v.emplace_back(std::to_string(i));
    }
    auto actual = GetCapturedStdout();
    EXPECT_THAT(actual, HasSubstr("0"));
    EXPECT_THAT(actual, HasSubstr("constructor"));
    EXPECT_THAT(actual, Not(HasSubstr("copy")));
    EXPECT_THAT(actual, HasSubstr("move"));
    std::cout << std::endl << actual << std::endl;
}
