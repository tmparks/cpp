#pragma once
#include "Array.hpp"

// An array that can grow.
class StretchableArray : public Array
{
public:
    using Array::Array;      // inherit all constructors
    using Array::operator[]; // inherit all overloads
    double& operator[](gsl::index i) override;
};
