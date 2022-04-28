// See [What is “virtual data,” and how-can / why-would I use it in C++?]
// (https://isocpp.org/wiki/faq/value-vs-ref-semantics#virt-data)

#include "StretchableArray.hpp"
#include <iostream>

class Stack : private Verbose
{
public:
    void Push(double x);
    double Pop();

protected:
    // The base class has no public constructor and does not provide its own
    // object. Instead it accepts an object from a derived class.
    Stack(std::string&& name, Array& data);

private:
    // To simulate virtual data in C++, the base class has a reference to the
    // member object and the derived class’s destructor deletes the object.
    // No additional memory allocation overhead is imposed.
    Array& data_;
    int index_ { 0 };
};

////////////////////////////////////////////////////////////////////////////////

// Virtual data makes the implementation of the derived class easier:
// most of the implementation is inherited.
class FixedStack : public Stack
{
public:
    // The derived class has a constructor that provides an object to the base class.
    FixedStack(gsl::index size = 1);

private:
    // The derived class contains the member object.
    Array value_;
};

////////////////////////////////////////////////////////////////////////////////

// Virtual data makes the implementation of the derived class easier:
// most of the implementation is inherited.
class StretchableStack : public Stack
{
public:
    // The derived class has a constructor that provides an object to the base class.
    StretchableStack(gsl::index size = 1);

private:
    // The derived class contains the member object.
    StretchableArray value_;
};

////////////////////////////////////////////////////////////////////////////////

Stack::Stack(std::string&& name, Array& data) :
    Verbose(std::move(name)),
    data_(data)
{
    std::cout << gsl::czstring(__func__) << ": &data_=" << &data_ << std::endl;
}

void Stack::Push(double x)
{
    data_[index_++] = x; // may throw
}

double Stack::Pop()
{
    return data_[--index_]; // may throw
}

////////////////////////////////////////////////////////////////////////////////

FixedStack::FixedStack(gsl::index size) :
    Stack(gsl::czstring(__func__), value_),
    value_(size)
{
    std::cout << gsl::czstring(__func__) << ": &value_=" << &value_ << std::endl;
}

////////////////////////////////////////////////////////////////////////////////

StretchableStack::StretchableStack(gsl::index size) :
    Stack(gsl::czstring(__func__), value_),
    value_(size)
{
    std::cout << gsl::czstring(__func__) << ": &value_=" << &value_ << std::endl;
}

////////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

TEST(VirtualDataRef, Stack_Push_Pop)
{
    const gsl::index size { 3 };
    FixedStack stack(size);
    for (gsl::index x = 0; x < size; x++)
    {
        stack.Push(double(x));
    }
    for (gsl::index x = size - 1; x >= 0; x--)
    {
        EXPECT_EQ(double(x), stack.Pop());
    }
}

TEST(VirtualDataRef, Stack_Push_Full)
{
    gsl::index size { 3 };
    FixedStack stack(size);
    for (gsl::index x = 0; x < size; x++)
    {
        stack.Push(double(x));
    }
    EXPECT_THROW(stack.Push(double(size)), std::out_of_range);
}

TEST(VirtualDataRef, Stack_Pop_Empty)
{
    FixedStack stack(1);
    EXPECT_THROW(stack.Pop(), std::out_of_range);
}

TEST(VirtualDataRef, Stack_Stretch)
{
    gsl::index size { 3 };
    StretchableStack stack(size);
    for (gsl::index x = 0; x < size; x++)
    {
        stack.Push(double(x));
    }
    EXPECT_NO_THROW(stack.Push(double(size)));
    EXPECT_EQ(double(size), stack.Pop());
    for (gsl::index x = size - 1; x >= 0; x--)
    {
        EXPECT_EQ(double(x), stack.Pop());
    }
}
