#include "Array.hpp"
#include <iostream>

// default constructor
Array::Array() :
    Verbose { gsl::czstring(__func__) }
{
}

// constructor
Array::Array(gsl::index size) :
    Verbose { gsl::czstring(__func__) },
    size_ { size },
    data_ { std::make_unique<double[]>(size_) } // NOLINT(*-avoid-c-arrays)
{
}

// copy constructor (deep copy)
// explicitly invoke Verbose copy constructor
// instead of delegating to another constructor
Array::Array(const Array& other) :
    Verbose { other },
    size_ { other.size_ },
    data_ { std::make_unique<double[]>(size_) } // NOLINT(*-avoid-c-arrays)}
{
    const auto* begin = other.data_.get();
    const auto* end = begin + other.size_; // NOLINT *-pro-bounds-pointer-arithmetic
    auto* out = data_.get();
    std::copy(begin, end, out);
}

// move constructor
// explicitlly invoke Verbose move constructor
// instead of delgating to default constructor
// see [What is the copy-and-swap idiom?]
//     (https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom)
Array::Array(Array&& other) noexcept :
    Verbose { std::move(other) }
{
    swap(*this, other); // other becomes empty
}

// copy assignment operator
Array& Array::operator=(const Array& other)
{
    assign(other); // copy constructor may throw
    return *this;
}

// move assignment operator
Array& Array::operator=(Array&& other) noexcept
{
    assign(std::move(other)); // move constructor is noexcept
    return *this;
}

// unified assignment
// see [What is the copy-and-swap idiom?]
//     (https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom)
// see Item 11: [Handle assignment to self in operator=]
//     (https://learning.oreilly.com/library/view/effective-c-third/0321334876/ch02.html#ch02lev1sec7)
void Array::assign(Array other) noexcept // pass by value
{
    std::cout << name() << ": unified assignment" << std::endl;
    using std::swap; // enable argument dependent lookup
    swap(*this, other);
}

// non-member swap
void swap(Array& left, Array& right) noexcept
{
    using std::swap; // enable Argument Dependent Lookup
    swap(static_cast<Verbose&>(left), static_cast<Verbose&>(right));
    swap(left.size_, right.size_);
    swap(left.data_, right.data_);
}

gsl::index Array::size() const noexcept
{
    return size_;
}

const double& Array::operator[](gsl::index i) const
{
    check_bounds(i);
    return data_[i];
}

double& Array::operator[](gsl::index i)
{
    reserve(i);
    check_bounds(i);
    return data_[i];
}

// see ES.50: [Don't cast away const]
//     (https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#Res-casts-const)
// see Item 3: Use const whenever possible:
//     [Avoiding duplication in const and non-const member functions]
//     (https://learning.oreilly.com/library/view/effective-c-third/0321334876/ch01.html#ch01lev2sec2)
void Array::check_bounds(gsl::index i) const
{
    if (i >= size_)
    {
        throw std::out_of_range(
            "index exceeds size: "
            + std::to_string(i)
            + ">="
            + std::to_string(size_));
    }
    else if (i < 0)
    {
        throw std::out_of_range(
            "index is negative: "
            + std::to_string(i)
            + "< 0");
    }
}

void Array::reserve(gsl::index)
{
    // Does nothing in base class.
}

////////////////////////////////////////////////////////////////////////////////

#include <gmock/gmock.h>

using namespace testing;
using namespace testing::internal;

namespace // anonymous namespace for definitions that are local to this file
{
    class TestableArray : public Array
    {
    public:
        using Array::Array;
        using Array::size_;
        using Array::data_;
    };
} // anonymous namespace

TEST(Array, DefaultConstructor)
{
    auto a = TestableArray { }; // constructor
    EXPECT_EQ(0, a.size());
}

TEST(Array, MoveConstructor)
{
    constexpr auto size = gsl::index { 3 };
    auto a0 = TestableArray { size }; // constructor
    for (auto i = gsl::index { 0 }; i < size; i++)
    {
        a0[i] = gsl::narrow_cast<double>(2 * i); // even
    }

    CaptureStdout();
    auto a1 = std::move(a0); // move constructor
    auto actual = GetCapturedStdout();

    EXPECT_THAT(actual, HasSubstr("constructor"));
    EXPECT_THAT(actual, Not(HasSubstr("copy")));
    EXPECT_THAT(actual, HasSubstr("move"));
    EXPECT_THAT(actual, HasSubstr("swap"));
    std::cout << std::endl << actual << std::endl;

    const auto& const0 = a0;
    const auto& const1 = a1;
    EXPECT_EQ(0, const0.size_);
    EXPECT_EQ(nullptr, const0.data_);
    EXPECT_EQ(size, const1.size());
    for (auto i = gsl::index { 0 }; i < size; i++)
    {
        EXPECT_EQ(const1[i], 2 * i); // even
    }
}

TEST(Array, CopyAssignment)
{
    constexpr auto size = gsl::index { 3 };
    auto a0 = TestableArray { size }; // constructor
    auto a1 = TestableArray { size }; // constructor
    for (auto i = gsl::index { 0 }; i < size; i++)
    {
        a0[i] = gsl::narrow_cast<double>(2 * i);     // even
        a1[i] = gsl::narrow_cast<double>(2 * i + 1); // odd
    }

    CaptureStdout();
    a0 = a1; // copy assignment
    auto actual = GetCapturedStdout();

    EXPECT_THAT(actual, HasSubstr("constructor"));
    EXPECT_THAT(actual, HasSubstr("copy"));
    EXPECT_THAT(actual, Not(HasSubstr("move")));
    EXPECT_THAT(actual, HasSubstr("swap"));
    EXPECT_THAT(actual, HasSubstr("destructor"));
    std::cout << std::endl << actual << std::endl;

    const auto& const0 = a0;
    const auto& const1 = a1;
    EXPECT_NE(const0.data_, const1.data_); // pointers should differ
    EXPECT_EQ(const0.size(), const1.size());
    for (auto i = gsl::index { 0 }; i < size; i++)
    {
        EXPECT_EQ(const0[i], const1[i]);
    }
}

TEST(Array, MoveAssignment)
{
    constexpr auto size = gsl::index { 3 };
    auto a0 = TestableArray { size }; // constructor
    auto a1 = TestableArray { size }; // constructor
    for (auto i = gsl::index { 0 }; i < size; i++)
    {
        a0[i] = gsl::narrow_cast<double>(2 * i);     // even
        a1[i] = gsl::narrow_cast<double>(2 * i + 1); // odd
    }

    CaptureStdout();
    a0 = std::move(a1); // move assignment
    auto actual = GetCapturedStdout();

    EXPECT_THAT(actual, HasSubstr("constructor"));
    EXPECT_THAT(actual, Not(HasSubstr("copy")));
    EXPECT_THAT(actual, HasSubstr("move"));
    EXPECT_THAT(actual, HasSubstr("swap"));
    EXPECT_THAT(actual, HasSubstr("destructor"));
    std::cout << std::endl << actual << std::endl;

    const auto& const0 = a0;
    const auto& const1 = a1;
    for (auto i = gsl::index { 0 }; i < size; i++)
    {
        EXPECT_EQ(const0[i], 2 * i + 1); // odd
    }
    EXPECT_EQ(0, const1.size_);
    EXPECT_EQ(nullptr, const1.data_);
}
