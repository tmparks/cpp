#include "StretchableArray.hpp"
#include <gtest/gtest.h>

TEST(StretchableArray, Stretch) {
    auto size = gsl::index{3};
    auto array = StretchableArray{size};
    for (auto x = gsl::index{0}; x < size; x++) {
        array[x] = gsl::narrow_cast<double>(x);
    }
    EXPECT_EQ(array.size(), size);
    EXPECT_NO_THROW(array[size] = size);
    EXPECT_GT(array.size(), size);
    for (auto x = gsl::index{0}; x < size; x++) {
        EXPECT_EQ(array[x], x);
    }
    EXPECT_EQ(array[size], size);
}
