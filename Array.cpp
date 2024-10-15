#include "Array.hpp"
#include "compat/memory14.hpp"

// constructor
Array::Array(gsl::index size) :
        size_{size},
        data_{std::make_unique<double[]>(size_)} { // NOLINT(*-avoid-c-arrays)
}

// copy constructor (deep copy)
// explicitly invoke Verbose copy constructor
// instead of delegating to another constructor
Array::Array(const Array& other) :
        size_{other.size_},
        data_{std::make_unique<double[]>(size_)} { // NOLINT(*-avoid-c-arrays)}
    const auto* begin = other.data_.get();
    const auto* end =
            begin + other.size_; // NOLINT *-pro-bounds-pointer-arithmetic
    auto* out = data_.get();
    std::copy(begin, end, out);
}

// move constructor
// explicitlly invoke Verbose move constructor
// instead of delgating to default constructor
// see [What is the copy-and-swap idiom?]
//     (https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom)
Array::Array(Array&& other) noexcept {
    swap(*this, other); // other becomes empty
}

// copy assignment operator
Array& Array::operator=(const Array& other) {
    assign(other); // copy constructor may throw
    return *this;
}

// move assignment operator
Array& Array::operator=(Array&& other) noexcept {
    assign(std::move(other)); // move constructor is noexcept
    return *this;
}

// unified assignment
// see [What is the copy-and-swap idiom?]
//     (https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom)
// see Item 11: [Handle assignment to self in operator=]
//     (https://learning.oreilly.com/library/view/effective-c-third/0321334876/ch02.html#ch02lev1sec7)
void Array::assign(Array other) noexcept { // pass by value
    using std::swap; // enable argument dependent lookup
    swap(*this, other);
}

// non-member swap
void swap(Array& left, Array& right) noexcept {
    using std::swap; // enable Argument Dependent Lookup
    swap(left.size_, right.size_);
    swap(left.data_, right.data_);
}

gsl::index Array::size() const noexcept { return size_; }

const double& Array::operator[](gsl::index i) const {
    check_bounds(i);
    return data_[i];
}

double& Array::operator[](gsl::index i) {
    reserve(i);
    check_bounds(i);
    return data_[i];
}

// see ES.50: [Don't cast away const]
//     (https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#Res-casts-const)
// see Item 3: Use const whenever possible:
//     [Avoiding duplication in const and non-const member functions]
//     (https://learning.oreilly.com/library/view/effective-c-third/0321334876/ch01.html#ch01lev2sec2)
void Array::check_bounds(gsl::index i) const {
    if (i >= size_) {
        throw std::out_of_range(
                "index exceeds size: " + std::to_string(i)
                + ">=" + std::to_string(size_));
    }
    else if (i < 0) {
        throw std::out_of_range("index is negative: " + std::to_string(i) + "< 0");
    }
}

void Array::reserve(gsl::index) {
    // Does nothing in base class.
}
