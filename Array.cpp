#include "Array.hpp"
#include <gsl/gsl>

Array::Array(gsl::index size) :
    Array { gsl::czstring(__func__), size }
{
}

gsl::index Array::Size() const
{
    return size_;
}

double& Array::operator[](gsl::index i)
{
    CheckBounds(i);
    return data_[i];
}

Array::Array(const std::string& name, gsl::index size) :
    Verbose { name },
    size_ { size },
    data_ { std::make_unique<double[]>(size_) } // NOLINT(cppcoreguidelines-avoid-c-arrays)
{
}

void Array::CheckBounds(gsl::index i)
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
