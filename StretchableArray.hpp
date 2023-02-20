#pragma once
#include "Array.hpp"

// An array that can grow.
class StretchableArray : public Array {
public:
    using Array::Array; // inherit all constructors

private:
    void reserve(gsl::index i) override;
};
