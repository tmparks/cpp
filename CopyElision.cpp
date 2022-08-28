// See [Copy elision](https://en.cppreference.com/w/cpp/language/copy_elision)
// See [Return value optimizations](https://www.fluentcpp.com/2016/11/28/return-value-optimizations/)
// See [Understanding when not to std::move in C++](https://developers.redhat.com/blog/2019/04/12/understanding-when-not-to-stdmove-in-c)

#include "Verbose.hpp"

class Uncopyable : public Verbose
{
public:
    using Verbose::Verbose;                                 // inherit constructors
    Uncopyable(Uncopyable&&) noexcept = default;            // move constructor
    Uncopyable& operator=(Uncopyable&&) noexcept = default; // move assignment
    ~Uncopyable() override = default;                       // destructor

    Uncopyable() = delete;                                  // no default constructor
    Uncopyable(const Uncopyable&) = delete;                 // no copy constructor
    Uncopyable& operator=(const Uncopyable&) = delete;      // no copy assignment
};

class Unmovable : public Verbose
{
public:
    using Verbose::Verbose;                              // inherit constructors
    Unmovable(const Unmovable&) = default;               // copy constructor
    Unmovable& operator=(const Unmovable&) = default;    // copy assignment
    ~Unmovable() override = default;                     // destructor

    Unmovable() = delete;                                // no default constructor
    Unmovable(Unmovable&&) noexcept = delete;            // no move constructor
    Unmovable& operator=(Unmovable&&) noexcept = delete; // no move assignment
};

// Unnamed value constructed in return statement.
Verbose rvo()
{
    return Verbose { "unnamed" };
}

// Unnamed value constructed in return statement.
// Copy constructor need not be available.
Uncopyable rvo_uncopyable()
{
    return Uncopyable { "unnamed" };
}

// Unnamed value constructed in return statement.
// Move constructor need not be available.
Unmovable rvo_unmovable()
{
    return Unmovable { "unnamed" };
}

// Unnamed values constructed in all return statements.
Verbose rvo_conditional(int x)
{
    if (x % 2 == 0)
    {
        return Verbose { "even" };
    }
    else
    {
        return Verbose { "odd" };
    }
}

Verbose rvo_chained()
{
    return Verbose { rvo() };
}

// Unique named return value.
Verbose nrvo()
{
    Verbose result { "unique" };
    return result;
}

// Unique named movable return value.
// Copy constructor not available?!
Uncopyable nrvo_uncopyable()
{
    Uncopyable result { "unique" };
    return result;
}

// Unique named copyable return value.
// Oops! Move constructor not available! Does not compile!
/*
Unmovable nrvo_unmovable()
{
    Unmovable result { "unique" };
    return result;
}
*/

// Unique named return value in all return statements.
Verbose nrvo_conditional(int x)
{
    Verbose result { "unknown" };
    if (x % 2 == 0)
    {
        result.name() = "even";
        return result;
    }
    else
    {
        result.name() = "odd";
        return result;
    }
}

Verbose nrvo_chained()
{
    Verbose result { nrvo() };
    return result;
}

// Oops! Non-unique return values.
Verbose nrvo_not_unique(int x)
{
    if (x % 2 == 0)
    {
        Verbose even { "even" };
        return even;
    }
    else
    {
        Verbose odd { "odd" };
        return odd;
    }
}

// Oops! Return value is function parameter.
Verbose nrvo_value_parameter(Verbose v)
{
    return v;
}

// Oops! Return value is function parameter.
Verbose nrvo_const_ref_parameter(const Verbose& v)
{
    return v;
}

// Oops! Return value is function parameter.
Verbose nrvo_move_parameter(Verbose&& v)
{
    return v;
}

////////////////////////////////////////////////////////////////////////////////

#include <gmock/gmock.h>

using namespace testing;
using namespace testing::internal;

constexpr int even_number { 2 };
constexpr int odd_number { 17 };

TEST(CopyElision, rvo)
{
    CaptureStdout();
    auto v = rvo();
    auto actual = GetCapturedStdout();
    EXPECT_THAT(actual, Not(AnyOf(HasSubstr("copy"), HasSubstr("move"))));
    std::cout << std::endl << actual << std::endl;
}

TEST(CopyElision, rvo_uncopyable)
{
    CaptureStdout();
    auto u = rvo_uncopyable();
    auto actual = GetCapturedStdout();
    EXPECT_THAT(actual, Not(AnyOf(HasSubstr("copy"), HasSubstr("move"))));
    std::cout << std::endl << actual << std::endl;
}

TEST(CopyElision, rvo_unmovable)
{
    CaptureStdout();
    auto u = rvo_unmovable();
    auto actual = GetCapturedStdout();
    EXPECT_THAT(actual, Not(AnyOf(HasSubstr("copy"), HasSubstr("move"))));
    std::cout << std::endl << actual << std::endl;
}

TEST(CopyElision, rvo_conditional)
{
    CaptureStdout();
    auto v = rvo_conditional(even_number);
    auto actual = GetCapturedStdout();
    EXPECT_THAT(actual, Not(AnyOf(HasSubstr("copy"), HasSubstr("move"))));
    EXPECT_THAT(v.name(), StrEq("even"));
    std::cout << std::endl << actual << std::endl;
}

TEST(CopyElision, rvo_chained)
{
    CaptureStdout();
    auto v = rvo_chained();
    auto actual = GetCapturedStdout();
    EXPECT_THAT(actual, Not(AnyOf(HasSubstr("copy"), HasSubstr("move"))));
    std::cout << std::endl << actual << std::endl;
}

TEST(CopyElision, nrvo)
{
    CaptureStdout();
    auto v = nrvo();
    auto actual = GetCapturedStdout();
    EXPECT_THAT(actual, Not(AnyOf(HasSubstr("copy"), HasSubstr("move"))));
    std::cout << std::endl << actual << std::endl;
}

TEST(CopyElision, nrvo_uncopyable)
{
    CaptureStdout();
    auto v = nrvo_uncopyable();
    auto actual = GetCapturedStdout();
    EXPECT_THAT(actual, Not(AnyOf(HasSubstr("copy"), HasSubstr("move"))));
    std::cout << std::endl << actual << std::endl;
}

TEST(CopyElision, nrvo_conditional)
{
    CaptureStdout();
    auto v = nrvo_conditional(odd_number);
    auto actual = GetCapturedStdout();
    EXPECT_THAT(actual, Not(AnyOf(HasSubstr("copy"), HasSubstr("move"))));
    EXPECT_THAT(v.name(), StrEq("odd"));
    std::cout << std::endl << actual << std::endl;
}

TEST(CopyElision, nrvo_chained)
{
    CaptureStdout();
    auto v = nrvo_chained();
    auto actual = GetCapturedStdout();
    EXPECT_THAT(actual, Not(AnyOf(HasSubstr("copy"), HasSubstr("move"))));
    std::cout << std::endl << actual << std::endl;
}

TEST(CopyElision, nrvo_not_unique)
{
    CaptureStdout();
    auto v = nrvo_not_unique(odd_number);
    auto actual = GetCapturedStdout();
    EXPECT_THAT(actual, AnyOf(HasSubstr("copy"), HasSubstr("move")));
    EXPECT_THAT(v.name(), StrEq("odd"));
    std::cout << std::endl << actual << std::endl;
}

TEST(CopyElision, nrvo_parameter)
{
    auto p = Verbose { "parameter" };
    CaptureStdout();
    auto v1 = nrvo_value_parameter(p);
    auto actual = GetCapturedStdout();
    EXPECT_THAT(actual, AnyOf(HasSubstr("copy"), HasSubstr("move")));
    std::cout << std::endl << actual << std::endl;

    CaptureStdout();
    auto v2 = nrvo_const_ref_parameter(p);
    actual = GetCapturedStdout();
    EXPECT_THAT(actual, AnyOf(HasSubstr("copy"), HasSubstr("move")));
    std::cout << std::endl << actual << std::endl;

    CaptureStdout();
    auto v3 = nrvo_move_parameter(std::move(p));
    actual = GetCapturedStdout();
    EXPECT_THAT(actual, AnyOf(HasSubstr("copy"), HasSubstr("move")));
    std::cout << std::endl << actual << std::endl;
}

TEST(CopyElision, initialization)
{
    CaptureStdout();
    Verbose v1 = Verbose { "one" };
    auto v2 = Verbose { "two" };
    auto actual = GetCapturedStdout();
    EXPECT_THAT(actual, Not(AnyOf(HasSubstr("copy"), HasSubstr("move"))));
    std::cout << std::endl << actual << std::endl;
}
