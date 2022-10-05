// See [Copy elision](https://en.cppreference.com/w/cpp/language/copy_elision)
// See [Return value optimizations](https://www.fluentcpp.com/2016/11/28/return-value-optimizations/)
// See [Understanding when not to std::move in C++](https://developers.redhat.com/blog/2019/04/12/understanding-when-not-to-stdmove-in-c)

#include "Verbose.hpp"
#include <tuple>

// A class that can be moved but not copied.
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

// A class that can be copied but not moved.
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

// Unnamed value constructed from return value in return statement.
Verbose rvo_chained()
{
    return Verbose { rvo() };
}

// Unnamed tuple of unnamed values constructed in return statement.
auto rvo_tuple()
{
    return std::tuple<Verbose, Verbose>("first", "second");
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

// Unique named return value constructed from return value.
Verbose nrvo_chained()
{
    Verbose result { nrvo() };
    return result;
}

// Unique named tuple return value.
auto nrvo_tuple_good()
{
    auto result = std::tuple<Verbose, Verbose>("one", "two");
    auto &v1 = get<0>(result);
    auto &v2 = get<1>(result);
    v1.name() = "first";
    v2.name() = "second";
    return result;
}

// Unique named tuple return value.
auto nrvo_tuple_better()
{
    auto result = std::tuple<Verbose, Verbose>("one", "two");
    auto &[v1, v2] = result;
    v1.name() = "first";
    v2.name() = "second";
    return result;
}

// Fail! Named tuple of named values.
auto nrvo_tuple_bad()
{
    auto v1 = Verbose { "first" };
    auto v2 = Verbose { "second" };
    auto result = std::make_tuple(v1, v2);
    return result;
}

// Fail! Non-unique return values.
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

// Fail! Return value is function parameter.
Verbose nrvo_value_parameter(Verbose v)
{
    return v;
}

// Fail! Return value is function parameter.
Verbose nrvo_const_ref_parameter(const Verbose& v)
{
    return v;
}

// Fail! Return value is function parameter.
Verbose nrvo_move_parameter(Verbose&& v)
{
    return v;
}

// Fail! Unnamed value assigned to output parameter.
void rvo_output_parameter(Uncopyable& result)
{
    result = Uncopyable { "parameter" };
}

// Fail! Unique named value assigned to output parameter.
void nrvo_output_parameter(Unmovable& result)
{
    auto v = Unmovable{ "parameter" };
    result = v;
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
    auto p = Verbose { "local" };
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

TEST(CopyElision, output_parameter)
{
    auto name = std::string { "local" };
    auto p1 = Uncopyable { name };
    CaptureStdout();
    rvo_output_parameter(p1);
    auto actual = GetCapturedStdout();
    EXPECT_THAT(p1.name(), Not(StrEq(name)));
    EXPECT_THAT(actual, AnyOf(HasSubstr("copy"), HasSubstr("move")));
    std::cout << std::endl << actual << std::endl;

    auto p2 = Unmovable { name };
    CaptureStdout();
    nrvo_output_parameter(p2);
    actual = GetCapturedStdout();
    EXPECT_THAT(p2.name(), Not(StrEq(name)));
    EXPECT_THAT(actual, AnyOf(HasSubstr("copy"), HasSubstr("move")));
    std::cout << std::endl << actual << std::endl;
}

TEST(CopyElision, rvo_tuple_get)
{
    // Good: use get to access tuple elements by reference
    // without copying or moving.
    CaptureStdout();
    auto result = rvo_tuple();
    auto& v1 = std::get<0>(result);
    auto& v2 = std::get<1>(result);
    auto actual = GetCapturedStdout();
    EXPECT_THAT(v1.name(), StrEq("first"));
    EXPECT_THAT(v2.name(), StrEq("second"));
    EXPECT_THAT(actual, Not(AnyOf(HasSubstr("copy"), HasSubstr("move"))));
    std::cout << std::endl << actual << std::endl;
}

TEST(CopyElision, rvo_tuple_structured_binding)
{
    // Better: use structured binding to access tuple elements.
    // Same efficiency, less verbose.
    CaptureStdout();
    auto [v1, v2] = rvo_tuple();
    auto actual = GetCapturedStdout();
    EXPECT_THAT(v1.name(), StrEq("first"));
    EXPECT_THAT(v2.name(), StrEq("second"));
    EXPECT_THAT(actual, Not(AnyOf(HasSubstr("copy"), HasSubstr("move"))));
    std::cout << std::endl << actual << std::endl;
}

TEST(CopyElision, rvo_tuple_tie)
{
    // Bad: use tie to access tuple elements.
    // Constructor invoked for each local variable.
    // Assignment operator invoked for each local variable.
    auto v1 = Verbose { "one" };
    auto v2 = Verbose { "two" };
    CaptureStdout();
    std::tie(v1, v2) = rvo_tuple();
    auto actual = GetCapturedStdout();
    EXPECT_THAT(v1.name(), StrEq("first"));
    EXPECT_THAT(v2.name(), StrEq("second"));
    EXPECT_THAT(actual, Not(AnyOf(
        HasSubstr("copy constructor"),
        HasSubstr("move constructor"))));
    EXPECT_THAT(actual, HasSubstr("assignment"));
    std::cout << std::endl << actual << std::endl;
}

TEST(CopyElision, nrvo_tuple_get)
{
    // Good: use get to access tuple elements by reference
    // without copying or moving.
    CaptureStdout();
    auto result = nrvo_tuple_good();
    auto &v1 = std::get<0>(result);
    auto &v2 = std::get<1>(result);
    auto actual = GetCapturedStdout();
    EXPECT_THAT(v1.name(), StrEq("first"));
    EXPECT_THAT(v2.name(), StrEq("second"));
    EXPECT_THAT(actual, Not(AnyOf(HasSubstr("copy"), HasSubstr("move"))));
    std::cout << std::endl << actual << std::endl;
}

TEST(CopyElision, nrvo_tuple_structured_binding)
{
    // Better: use structured binding to access tuple elements.
    // Same efficiency, less verbose.
    CaptureStdout();
    auto [v1, v2] = nrvo_tuple_better();
    auto actual = GetCapturedStdout();
    EXPECT_THAT(v1.name(), StrEq("first"));
    EXPECT_THAT(v2.name(), StrEq("second"));
    EXPECT_THAT(actual, Not(AnyOf(HasSubstr("copy"), HasSubstr("move"))));
    std::cout << std::endl << actual << std::endl;
}

TEST(CopyElision, nrvo_tuple_tie)
{
    // Bad: use tie to access tuple elements.
    // Local variables must be constructed before call.
    // Results are assigned to local variables.
    auto v1 = Verbose { "one" };
    auto v2 = Verbose { "two" };
    CaptureStdout();
    std::tie(v1, v2) = nrvo_tuple_better();
    auto actual = GetCapturedStdout();
    EXPECT_THAT(v1.name(), StrEq("first"));
    EXPECT_THAT(v2.name(), StrEq("second"));
    EXPECT_THAT(actual, Not(AnyOf(
        HasSubstr("copy constructor"),
        HasSubstr("move constructor"))));
    EXPECT_THAT(actual, HasSubstr("assignment"));
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
