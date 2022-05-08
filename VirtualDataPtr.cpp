// See [What is “virtual data,” and how-can / why-would I use it in C++?]
// (https://isocpp.org/wiki/faq/value-vs-ref-semantics#virt-data)

#include "StretchableArray.hpp"

class StackPtr : private Verbose
{
public:
    // The base class has a normal constructor that provides its own object.
    explicit StackPtr(gsl::index size = 1);

public:
    void Push(double x);
    double Pop();

protected:
    // The base class has a constructor that accepts an object from a derived class.
    StackPtr(const std::string& name, std::unique_ptr<Array> data);

private:
    // To simulate virtual data in C++, the base class has a pointer to the
    // member object and the base class’s destructor deletes the object.
    // Some additional memory allocation overhead is imposed.
    std::unique_ptr<Array> data_;
    gsl::index index_ { 0 };
};

////////////////////////////////////////////////////////////////////////////////

// Virtual data makes the implementation of the derived class easier:
// most of the implementation is inherited.
class StretchableStackPtr : public StackPtr
{
public:
    // The derived class has a constructor that provides a new object to the base class.
    explicit StretchableStackPtr(gsl::index size = 1);
};

////////////////////////////////////////////////////////////////////////////////

StackPtr::StackPtr(gsl::index size) :
    StackPtr(gsl::czstring(__func__), std::make_unique<Array>(size))
{
}

StackPtr::StackPtr(const std::string& name, std::unique_ptr<Array> data) :
    Verbose { name },
    data_ { std::move(data) }
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
    StackPtr {
        gsl::czstring(__func__), std::make_unique<StretchableArray>(size) }
{
}

////////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

TEST(VirtualDataPtr, Stack_Push_Pop)
{
    const auto size = gsl::index { 3 };
    auto stack = StackPtr { size };
    for (auto x = gsl::index { 0 }; x < size; x++)
    {
        stack.Push(gsl::narrow_cast<double>(x));
    }
    for (auto x = size - 1; x >= 0; x--)
    {
        EXPECT_EQ(gsl::narrow_cast<double>(x), stack.Pop());
    }
}

TEST(VirtualDataPtr, Stack_Push_Full)
{
    const auto size = gsl::index { 3 };
    auto stack = StackPtr { size };
    for (auto x = gsl::index { 0 }; x < size; x++)
    {
        stack.Push(gsl::narrow_cast<double>(x));
    }
    EXPECT_THROW(stack.Push(gsl::narrow_cast<double>(size)), std::out_of_range);
}

TEST(VirtualDataPtr, Stack_Pop_Empty)
{
    auto stack = StackPtr { 1 };
    EXPECT_THROW(stack.Pop(), std::out_of_range);
}

TEST(VirtualDataPtr, Stack_Stretch)
{
    auto size = gsl::index(3);
    auto stack = StretchableStackPtr { size };
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
