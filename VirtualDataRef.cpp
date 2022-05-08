// See [What is “virtual data,” and how-can / why-would I use it in C++?]
// (https://isocpp.org/wiki/faq/value-vs-ref-semantics#virt-data)

#include "StretchableArray.hpp"
#include <iostream>

class StackRef : private Verbose
{
public:
    void Push(double x);
    double Pop();

protected:
    // The base class has no public constructor and does not provide its own
    // object. Instead it accepts an object from a derived class.
    StackRef(const std::string& name, Array& data);

private:
    // To simulate virtual data in C++, the base class has a reference to the
    // member object and the derived class’s destructor deletes the object.
    // No additional memory allocation overhead is imposed.
    Array& data_;
    gsl::index index_ { 0 };
};

////////////////////////////////////////////////////////////////////////////////

// Virtual data makes the implementation of the derived class easier:
// most of the implementation is inherited.
class FixedStackRef : public StackRef
{
public:
    // The derived class has a constructor that provides an object to the base class.
    explicit FixedStackRef(gsl::index size = 1);

private:
    // The derived class contains the member object.
    Array value_;
};

////////////////////////////////////////////////////////////////////////////////

// Virtual data makes the implementation of the derived class easier:
// most of the implementation is inherited.
class StretchableStackRef : public StackRef
{
public:
    // The derived class has a constructor that provides an object to the base class.
    explicit StretchableStackRef(gsl::index size = 1);

private:
    // The derived class contains the member object.
    StretchableArray value_;
};

////////////////////////////////////////////////////////////////////////////////

StackRef::StackRef(const std::string& name, Array& data) :
    Verbose { name },
    data_ { data }
{
    std::cout << gsl::czstring(__func__) << ": &data_=" << &data_ << std::endl;
}

void StackRef::Push(double x)
{
    data_[index_++] = x; // may throw
}

double StackRef::Pop()
{
    return data_[--index_]; // may throw
}

////////////////////////////////////////////////////////////////////////////////

FixedStackRef::FixedStackRef(gsl::index size) :
    StackRef { gsl::czstring(__func__), value_ },
    value_ { size }
{
    std::cout << gsl::czstring(__func__) << ": &value_=" << &value_ << std::endl;
}

////////////////////////////////////////////////////////////////////////////////

StretchableStackRef::StretchableStackRef(gsl::index size) :
    StackRef { gsl::czstring(__func__), value_ },
    value_ { size }
{
    std::cout << gsl::czstring(__func__) << ": &value_=" << &value_ << std::endl;
}

////////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

TEST(VirtualDataRef, Stack_Push_Pop)
{
    const auto size = gsl::index { 3 };
    auto stack = FixedStackRef { size };
    for (auto x = gsl::index { 0 }; x < size; x++)
    {
        stack.Push(gsl::narrow_cast<double>(x));
    }
    for (auto x = size - 1; x >= 0; x--)
    {
        EXPECT_EQ(gsl::narrow_cast<double>(x), stack.Pop());
    }
}

TEST(VirtualDataRef, Stack_Push_Full)
{
    auto size = gsl::index { 3 };
    auto stack = FixedStackRef { size };
    for (auto x = gsl::index { 0 }; x < size; x++)
    {
        stack.Push(gsl::narrow_cast<double>(x));
    }
    EXPECT_THROW(stack.Push(gsl::narrow_cast<double>(size)), std::out_of_range);
}

TEST(VirtualDataRef, Stack_Pop_Empty)
{
    auto stack = FixedStackRef { 1 };
    EXPECT_THROW(stack.Pop(), std::out_of_range);
}

TEST(VirtualDataRef, Stack_Stretch)
{
    const auto size = gsl::index { 3 };
    auto stack = StretchableStackRef { size };
    for (auto x = gsl::index { 0 }; x < size; x++)
    {
        stack.Push(gsl::narrow_cast<double>(x));
    }
    EXPECT_NO_THROW(stack.Push(gsl::narrow_cast<double>(size)));
    EXPECT_EQ(gsl::narrow_cast<double>(size), stack.Pop());
    for (auto x = size - 1; x >= 0; x--)
    {
        EXPECT_EQ(gsl::narrow_cast<double>(x), stack.Pop());
    }
}
