#include "Shape.hpp"
#include "compat/gsl14.hpp"
#include <gtest/gtest.h>

namespace { // anonymous namespace for definitions that are local to this file

    void TestClone(const Shape& original) {
        std::cout << gsl::czstring { __func__ } << ": "
                  << typeid(original).name() << std::endl;
        auto clone = original.clone();
        EXPECT_NE(clone.get(), &original) << "distinct objects";

        // Necessary (but not sufficient) conditions for two shapes to be the same.
        EXPECT_DOUBLE_EQ(clone->area(), original.area());
        EXPECT_DOUBLE_EQ(
                clone->boundingCircle().area(), original.boundingCircle().area());
        EXPECT_DOUBLE_EQ(
                clone->boundingBox().area(), original.boundingBox().area());
    }

    // NOLINTBEGIN(*-avoid-magic-numbers)

    void TestScale(const Shape& shape) {
        auto area1 = shape.clone()->scaleTo(1.0).area();
        auto area2 = shape.clone()->scaleTo(2.0).area();
        auto area3 = shape.clone()->scaleTo(3.0).scaleBy(4.0).area();
        EXPECT_DOUBLE_EQ(4.0 * area1, area2);
        EXPECT_DOUBLE_EQ(36.0 * area2, area3);
    }

    // NOLINTEND(*-avoid-magic-numbers)

} // anonymous namespace

TEST(Shape, clone) {
    TestClone(Rectangle(3, 4));
    TestClone(Square(2));
    TestClone(Ellipse(3, 4));
    TestClone(Circle(2));
}

TEST(Shape, scale) {
    TestScale(Rectangle(3, 4));
    TestScale(Square(2));
    TestScale(Ellipse(3, 4));
    TestScale(Circle(2));
}
