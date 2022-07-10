#include "Array.hpp"
#include "utility.hpp"
#include <iostream>

// default constructor
Array::Array() :
    Array { 0 }
{
}

// constructor
Array::Array(gsl::index size) :
    Verbose { gsl::czstring(__func__) },
    size_ { size },
    data_ { std::make_unique<double[]>(size_) }
{
}

// copy constructor (deep copy)
Array::Array(const Array& other) :
    Array { other.size_ }
{
    const auto* begin = other.data_.get();
    const auto* end = begin + other.size_;
    auto* out = data_.get();
    std::copy(begin, end, out);
}

// move constructor
// explicitlly invoke Verbose move constructor
// see [What is the copy-and-swap idiom?]
//     (https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom)
Array::Array(Array&& other) :
    Verbose { std::move(other) },
    size_ { 0 },
    data_ { }
{
    swap(*this, other); // other becomes empty
}

// unified assignment operator
// see [What is the copy-and-swap idiom?]
//     (https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom)
// see Item 11: [Handle assignment to self in operator=]
//     (https://learning.oreilly.com/library/view/effective-c-third/0321334876/ch02.html#ch02lev1sec7)
Array& Array::operator=(Array other)
{
    std::cout << name_ << ": unified assignment" << std::endl;
    using std::swap; // enable argument dependent lookup
    swap(*this, other);
    return *this;
}

// non-member swap
void swap(Array& left, Array& right) noexcept
{
    std::cout << left.name_ << ": swap" << std::endl;
    using std::swap; // enable Argument Dependent Lookup
    swap(left.size_, right.size_);
    swap(left.data_, right.data_);
}

gsl::index Array::size() const
{
    return size_;
}

const double& Array::operator[](gsl::index i) const
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
    return data_[i];
}

// see Item 3: Use const whenever possible:
//     [Avoiding duplication in const and non-const member functions]
//     (https://learning.oreilly.com/library/view/effective-c-third/0321334876/ch01.html#ch01lev2sec2)
double& Array::operator[](gsl::index i)
{
    return as_mutable(std::as_const(*this)[i]);
}

////////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

namespace // anonymous namespace for definitions that are local to this file
{
    class TestableArray : public Array
    {
    public:
        using Array::Array;
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
        a0[i] = 2 * i;     // even
    }
    auto a1 = std::move(a0); // move constructor
    EXPECT_EQ(0, a0.size());
    EXPECT_EQ(nullptr, a0.data_);
    EXPECT_EQ(size, a1.size());
    for (auto i = gsl::index { 0 }; i < size; i++)
    {
        EXPECT_EQ(a1[i], 2 * i);     // even
    }
}

TEST(Array, CopyAssignment)
{
    constexpr auto size = gsl::index { 3 };
    auto a0 = TestableArray { size }; // constructor
    auto a1 = TestableArray { size }; // constructor
    for (auto i = gsl::index { 0 }; i < size; i++)
    {
        a0[i] = 2 * i;     // even
        a1[i] = 2 * i + 1; // odd
    }
    a0 = a1; // copy assignment
    EXPECT_NE(a0.data_, a1.data_); // pointers should differ
    for (auto i = gsl::index { 0 }; i < size; i++)
    {
        EXPECT_EQ(a0[i], a1[i]);
    }
}

TEST(Array, MoveAssignment)
{
    constexpr auto size = gsl::index { 3 };
    auto a0 = TestableArray { size }; // constructor
    auto a1 = TestableArray { size }; // constructor
    for (auto i = gsl::index { 0 }; i < size; i++)
    {
        a0[i] = 2 * i;     // even
        a1[i] = 2 * i + 1; // odd
    }
    a0 = std::move(a1); // move assignment
    for (auto i = gsl::index { 0 }; i < size; i++)
    {
        EXPECT_EQ(a0[i], 2 * i + 1); // odd
    }
    EXPECT_EQ(0, a1.size());
    EXPECT_EQ(nullptr, a1.data_);
}
