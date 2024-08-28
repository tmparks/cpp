#include "Shape.hpp"

#include "compat/gsl14.hpp"
#include "compat/numbers20.hpp"

#include <algorithm>
#include <cmath>
#include <numbers>

////////////////////////////////////////////////////////////////////////////////

// public static

Point Point::rectangular(double x, double y) { return Point { x, y }; }

Point Point::polar(double radius, double angle) {
    return Point { radius * std::cos(angle), radius * std::sin(angle) };
}

// public const

double Point::x() const { return x_; }

double Point::y() const { return y_; }

double Point::radius() const { return std::hypot(x_, y_); }

double Point::angle() const { return std::atan2(y_, x_); }

// private

Point::Point(double x, double y) : x_ { x }, y_ { y } { }

// non-member

double distance(const Point& a, const Point& b) {
    return std::hypot(a.x() - b.x(), a.y() - b.y());
}

////////////////////////////////////////////////////////////////////////////////

// public const

std::unique_ptr<Shape> Shape::clone() const {
    auto* result = cloneImpl();
    // If a derived class fails to override cloneImpl,
    // then the returned object will be of the wrong type.
    Ensures(typeid(*result) == typeid(*this));
    return std::unique_ptr<Shape> { result };
}

double Shape::area() const { return areaImpl(scale_); }

Circle Shape::boundingCircle() const {
    auto result = Circle { boundingRadiusImpl(scale_) };
    result.moveTo(position_.x(), position_.y());
    return result;
}

Rectangle Shape::boundingBox() const {
#if __cplusplus >= 201703L
    auto [width, height] = boundingBoxImpl(scale_, rotation_);
#else
    auto tuple = boundingBoxImpl(scale_, rotation_);
    auto& width = std::get<0>(tuple);
    auto& height = std::get<1>(tuple);
#endif // C++17
    auto result = Rectangle { width, height };
    result.moveTo(position_.x(), position_.y());
    return result;
}

// public

Shape& Shape::scaleTo(double s) {
    scale_ = s;
    return *this;
}

Shape& Shape::scaleBy(double ds) {
    scale_ *= ds;
    return *this;
}

Shape& Shape::rotateTo(double radians) {
    rotation_ = std::fmod(radians, 2 * std::numbers::pi);
    return *this;
}

Shape& Shape::rotateBy(double dradians) {
    rotation_ = std::fmod(rotation_ + dradians, 2 * std::numbers::pi);
    return *this;
}

Shape& Shape::moveTo(double x, double y) {
    position_ = Point::rectangular(x, y);
    return *this;
}

Shape& Shape::moveBy(double dx, double dy) {
    position_ = Point::rectangular(position_.x() + dx, position_.y() + dy);
    return *this;
}

////////////////////////////////////////////////////////////////////////////////

// public

Rectangle::Rectangle(double width, double height) :
        width_ { width }, height_ { height } {
    Expects(width > 0.0);
    Expects(height > 0.0);
}

// private const override

Rectangle* Rectangle::cloneImpl() const {
    return new Rectangle { *this }; // NOLINT(*-owning-memory)
}

double Rectangle::areaImpl(double scale) const {
    return (scale * width_) * (scale * height_);
}

double Rectangle::boundingRadiusImpl(double scale) const {
    return std::hypot(scale * width_, scale * height_) / 2;
}

std::tuple<double, double> Rectangle::boundingBoxImpl(
        double scale, double rotation) const {
    // Corners of scaled rectangle.
    auto halfWidth = scale * width_ / 2;
    auto halfHeight = scale * height_ / 2;
    auto s1 = Point::rectangular(-halfWidth, -halfHeight);
    auto s2 = Point::rectangular(-halfWidth, +halfHeight);
    auto s3 = Point::rectangular(+halfWidth, -halfHeight);
    auto s4 = Point::rectangular(+halfWidth, +halfHeight);

    // Corners of scaled and rotated rectangle.
    auto r1 = Point::polar(s1.radius(), s1.angle() + rotation);
    auto r2 = Point::polar(s2.radius(), s2.angle() + rotation);
    auto r3 = Point::polar(s3.radius(), s3.angle() + rotation);
    auto r4 = Point::polar(s4.radius(), s4.angle() + rotation);

    // Find bounds.
    auto x = { r1.x(), r2.x(), r3.x(), r4.x() };
    auto y = { r1.y(), r2.y(), r3.y(), r4.y() };
    return { std::max(x) - std::min(x), std::max(y) - std::min(y) };
}

////////////////////////////////////////////////////////////////////////////////

// public

Square::Square(double side) : Rectangle { side, side } { }

// private const override

Square* Square::cloneImpl() const {
    return new Square { *this }; // NOLINT(*-owning-memory)
}

////////////////////////////////////////////////////////////////////////////////

// public

Ellipse::Ellipse(double width, double height) :
        a_ { std::max(width, height) / 2 },
        b_ { std::min(width, height) / 2 },
        e_ { std::sqrt(1.0 - std::pow(b_ / a_, 2)) } {
    Expects(width > 0.0);
    Expects(height > 0.0);
}

// private const override

Ellipse* Ellipse::cloneImpl() const {
    return new Ellipse { *this }; // NOLINT(*-owning-memory)
}

double Ellipse::areaImpl(double scale) const {
    return std::numbers::pi * (scale * a_) * (scale * b_);
}

double Ellipse::boundingRadiusImpl(double scale) const { return scale * a_; }

std::tuple<double, double> Ellipse::boundingBoxImpl(
        double scale, double rotation) const {
    return { 2 * scale * radius(rotation),
             2 * scale * radius(rotation + std::numbers::pi / 2) };
}

// private const

double Ellipse::radius(double angle) const {
    return b_ / std::sqrt(1.0 - std::pow(e_ * std::cos(angle), 2));
}

////////////////////////////////////////////////////////////////////////////////

// public

Circle::Circle(double radius) : Ellipse { 2 * radius, 2 * radius } { }

// private const override

Circle* Circle::cloneImpl() const {
    return new Circle { *this }; // NOLINT(*-owning-memory)
}

////////////////////////////////////////////////////////////////////////////////

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
