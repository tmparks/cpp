#include "StretchableArray.hpp"
#include "compat/memory14.hpp"

void StretchableArray::reserve(gsl::index i) {
    if (i >= size_) {
        auto larger_size = std::max(i, 2 * size_);
        // NOLINTNEXTLINE(*-avoid-c-arrays)
        auto larger_data = std::make_unique<double[]>(larger_size);
        std::copy(data_.get(), data_.get() + size_, larger_data.get());
        data_ = std::move(larger_data);
        size_ = larger_size;
    }
}
