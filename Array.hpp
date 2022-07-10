#pragma once
#include "Verbose.hpp"
#include <gsl/gsl>
#include <memory>

// A fixed-size array.
// This is an example of a wrapper for a dynamically allocated array,
// thus we cannot avoid using an array.
class Array : protected Verbose
{
public:
    Array();                                   // default constructor
    explicit Array(gsl::index size);           // constructor
    Array(const Array& other);                 // copy constructor
    Array(Array&& other);                      // move constructor
    Array& operator=(Array other);             // unified assignment
    ~Array() override = default;               // destructor
    friend void swap(Array&, Array&) noexcept; // non-member swap

    gsl::index size() const;
    const double& operator[](gsl::index) const;
    virtual double& operator[](gsl::index);

protected:
    gsl::index size_ { 0 }; // NOLINT(cppcoreguidelines-non-private-member-variables-in-classes)
    std::unique_ptr<double[]> data_; // NOLINT(cppcoreguidelines-avoid-c-arrays,cppcoreguidelines-non-private-member-variables-in-classes)
};
