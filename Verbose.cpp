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
    name_ = other.name_;
    std::cout << name_ << ": copy assignment" << std::endl;
    return *this;
}

Verbose& Verbose::operator=(Verbose&& other)
{
    name_ = other.name_;
    std::cout << name_ << ": move assignment" << std::endl;
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

class TestableVerbose : private Verbose
{
public:
    TestableVerbose() : Verbose(gsl::czstring(__func__)) {}
    ~TestableVerbose() override = default;
};

TEST(Verbose, Constructor)
{
    CaptureStdout();
    auto v1 = TestableVerbose();
    auto actual = GetCapturedStdout();
    EXPECT_THAT(actual, HasSubstr("constructor"));
    EXPECT_THAT(actual, Not(HasSubstr("copy")));
    EXPECT_THAT(actual, Not(HasSubstr("move")));
}
