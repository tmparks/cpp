#include "StretchableArray.hpp"
#include <iostream>

StretchableArray::StretchableArray(gsl::index size) :
    Array(gsl::czstring(__func__), size)
{
}

void StretchableArray::CheckBounds(gsl::index i)
{
    if (i >= size_)
    {
        Resize(std::max(i, 2*size_));
    }
    Array::CheckBounds(i);

}

void StretchableArray::Resize(gsl::index size)
{
    std::cout << name_ << ":" << gsl::czstring(__func__) << ": " << size_ << " to " << size << std::endl;
    auto temp { std::make_unique<double[]>(size) }; // NOLINT(cppcoreguidelines-avoid-c-arrays) 
    std::copy(
        data_.get(), 
        data_.get() + std::min(size, size_),
        temp.get());
    data_.reset(temp.release());
    size_ = size;
}
