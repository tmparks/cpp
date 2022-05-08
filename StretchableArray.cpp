#include "StretchableArray.hpp"
#include <iostream>

StretchableArray::StretchableArray(gsl::index size) :
    Array { gsl::czstring(__func__), size }
{
}

void StretchableArray::CheckBounds(gsl::index i)
{
    if (i >= size_)
    {
        Resize(std::max(i, 2 * size_));
    }
    Array::CheckBounds(i);
}

void StretchableArray::Resize(gsl::index size)
{
    using namespace std;
    std::cout << name_ << ":" << gsl::czstring(__func__) << ": " << size_ << " to " << size << std::endl;
    auto temp = std::make_unique<double[]>(size); // NOLINT(cppcoreguidelines-avoid-c-arrays) 
    std::copy(
        data_.get(),
        data_.get() + std::min(size, size_),
        temp.get());
    data_ = std::move(temp);
    size_ = size;
}

////////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

TEST(StretchableArray, Stretch)
{
    auto size = gsl::index { 3 };
    auto array = StretchableArray { size };
    for (auto x = gsl::index { 0 }; x < size; x++)
    {
        array[x] = gsl::narrow_cast<double>(x);
    }
    EXPECT_EQ(array.Size(), size);
    EXPECT_NO_THROW(array[size] = size);
    EXPECT_GT(array.Size(), size);
    for (auto x = gsl::index { 0 }; x < size; x++)
    {
        EXPECT_EQ(array[x], x);
    }
    EXPECT_EQ(array[size], size);
}
