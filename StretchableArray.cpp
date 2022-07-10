#include "StretchableArray.hpp"
#include <iostream>

double& StretchableArray::operator[](gsl::index i)
{
    if (i >= size_)
    {
        auto larger_size = std::max(i, 2 * size_);
        std::cout << name_ << ": resize from " << size_ << " to " << larger_size << std::endl;
        auto larger_data = std::make_unique<double[]>(larger_size); // NOLINT(cppcoreguidelines-avoid-c-arrays) 
        std::copy(data_.get(), data_.get() + size_, larger_data.get());
        data_ = std::move(larger_data);
        size_ = larger_size;
    }
    return Array::operator[](i);
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
    EXPECT_EQ(array.size(), size);
    EXPECT_NO_THROW(array[size] = size);
    EXPECT_GT(array.size(), size);
    for (auto x = gsl::index { 0 }; x < size; x++)
    {
        EXPECT_EQ(array[x], x);
    }
    EXPECT_EQ(array[size], size);
}
