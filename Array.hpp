#include "Verbose.hpp"
#include <gsl/gsl>
#include <memory>

/// A fixed-size array.
/// This is an example of a wrapper for a dynamically allocated array,
/// thus we cannot avoid using an array.
class Array : protected Verbose
{
public:
    Array(gsl::index size);
    gsl::index Size() const;
    double& operator[](gsl::index i);

protected:
    Array(std::string&& name, gsl::index size);
    virtual void CheckBounds(gsl::index i);
    gsl::index size_ { 0 }; // NOLINT(cppcoreguidelines-non-private-member-variables-in-classes)
    std::unique_ptr<double[]> data_; // NOLINT(cppcoreguidelines-avoid-c-arrays,cppcoreguidelines-non-private-member-variables-in-classes)
};
