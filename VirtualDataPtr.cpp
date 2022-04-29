// See [What is “virtual data,” and how-can / why-would I use it in C++?]
// (https://isocpp.org/wiki/faq/value-vs-ref-semantics#virt-data)

#include "StretchableArray.hpp"

class StackPtr : private Verbose
{
public:
    // The base class has a normal constructor that provides its own object.
    StackPtr(gsl::index size = 1);

public:
    void Push(double x);
    double Pop();

protected:
    // The base class has a constructor that accepts an object from a derived class.
    StackPtr(std::string&& name, std::unique_ptr<Array>&& data);

private:
    // To simulate virtual data in C++, the base class has a pointer to the
    // member object and the base class’s destructor deletes the object.
    // Some additional memory allocation overhead is imposed.
    std::unique_ptr<Array> data_;
    int index_ { 0 };
};

////////////////////////////////////////////////////////////////////////////////

// Virtual data makes the implementation of the derived class easier:
// most of the implementation is inherited.
class StretchableStackPtr : public StackPtr
{
public:
    // The derived class has a constructor that provides a new object to the base class.
    StretchableStackPtr(gsl::index size = 1);
};

////////////////////////////////////////////////////////////////////////////////

StackPtr::StackPtr(gsl::index size) :
    StackPtr(gsl::czstring(__func__), std::make_unique<Array>(size))
{
}

StackPtr::StackPtr(std::string&& name, std::unique_ptr<Array>&& data) :
    Verbose(std::move(name)),
    data_(std::move(data))
{
}

void StackPtr::Push(double x)
{
    (*data_)[index_++] = x; // may throw
}

double StackPtr::Pop()
{
    return (*data_)[--index_]; // may throw
}

////////////////////////////////////////////////////////////////////////////////

StretchableStackPtr::StretchableStackPtr(gsl::index size) :
    StackPtr(gsl::czstring(__func__), std::make_unique<StretchableArray>(size))
{
}

////////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

TEST(VirtualDataPtr, Stack_Push_Pop)
{
    const gsl::index size { 3 };
    StackPtr stack(size);
    for(gsl::index x = 0; x < size; x++)
    {
        stack.Push(double(x));
    }
    for(gsl::index x = size - 1; x >= 0; x--)
    {
        EXPECT_EQ(double(x), stack.Pop());
    }
}

TEST(VirtualDataPtr, Stack_Push_Full)
{
    gsl::index size { 3 };
    StackPtr stack(size);
    for(gsl::index x = 0; x < size; x++)
    {
        stack.Push(double(x));
    }
    EXPECT_THROW(stack.Push(double(size)), std::out_of_range);
}

TEST(VirtualDataPtr, Stack_Pop_Empty)
{
    StackPtr stack(1);
    EXPECT_THROW(stack.Pop(), std::out_of_range);
}

TEST(VirtualDataPtr, Stack_Stretch)
{
    gsl::index size { 3 };
    StretchableStackPtr stack(size);
    for(gsl::index x = 0; x < size; x++)
    {
        stack.Push(double(x));
    }
    EXPECT_NO_THROW(stack.Push(double(size)));
    EXPECT_EQ(double(size), stack.Pop());
    for(gsl::index x = size - 1; x >= 0; x--)
    {
        EXPECT_EQ(double(x), stack.Pop());
    }
}
