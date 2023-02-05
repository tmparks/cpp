#pragma once
#include "Verbose.hpp"
#include <gsl/gsl>
#include <memory>

// A fixed-size array.
// This is an example of a wrapper for a dynamically allocated array,
// similar to std::span.
class Array : protected Verbose
{
public:
    Array();                                   // default constructor
    explicit Array(gsl::index size);           // constructor
    Array(const Array& other);                 // copy constructor
    Array(Array&& other) noexcept;             // move constructor
    Array& operator=(const Array& other);      // copy assignment
    Array& operator=(Array&& other) noexcept;  // move assignment
    ~Array() override = default;               // destructor
    friend void swap(Array&, Array&) noexcept; // non-member swap

    gsl::index size() const noexcept;
    const double& operator[](gsl::index) const;
    double& operator[](gsl::index);

protected:
    void assign(Array other) noexcept;
    void check_bounds(gsl::index size) const;
    gsl::index size_ { 0 }; // NOLINT(*-non-private-member-variables-in-classes)
    std::unique_ptr<double[]> data_ { }; // NOLINT(*-avoid-c-arrays,*-non-private-member-variables-in-classes)

private:
    // See [Non-virtual interface pattern](https://en.wikipedia.org/wiki/Non-virtual_interface_pattern)
    // See [When should someone use private virtuals?](https://isocpp.org/wiki/faq/strange-inheritance#private-virtuals)
    // Derived classes can override this method, but cannot call it.
    virtual void reserve(gsl::index size);
};
