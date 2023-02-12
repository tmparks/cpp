#include "Shape.hpp"

#include <gsl/gsl>

#include <algorithm>
#include <cmath>
#include <numbers>

////////////////////////////////////////////////////////////////////////////////

// public static

Point Point::rectangular(double x, double y) { return Point(x, y); }

Point Point::polar(double radius, double angle) {
    return Point(radius * std::cos(angle), radius * std::sin(angle));
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

double Shape::area() const { return areaImpl(scale_); }

double Shape::boundingRadius() const { return boundingRadiusImpl(scale_); }

Rectangle Shape::boundingBox() const {
    auto result = boundingBoxImpl(scale_, rotation_);
    result.moveTo(position_.x(), position_.y());
    return result;
}

// public

void Shape::scaleTo(double s) { scale_ = s; }

void Shape::scaleBy(double ds) { scale_ *= ds; }

void Shape::rotateTo(double angle) {
    rotation_ = std::fmod(angle, 2 * std::numbers::pi);
}

void Shape::rotateBy(double dangle) {
    rotation_ = std::fmod(rotation_ + dangle, 2 * std::numbers::pi);
}

void Shape::moveTo(double x, double y) { position_ = Point::rectangular(x, y); }

void Shape::moveBy(double dx, double dy) {
    position_ = Point::rectangular(position_.x() + dx, position_.y() + dy);
}

////////////////////////////////////////////////////////////////////////////////

// public

Rectangle::Rectangle(double width, double height) :
        width_ { width }, height_ { height } {
    Expects(width > 0.0);
    Expects(height > 0.0);
}

// private const override

double Rectangle::areaImpl(double scale) const {
    return (scale * width_) * (scale * height_);
}

double Rectangle::boundingRadiusImpl(double scale) const {
    return std::hypot(scale * width_, scale * height_);
}

Rectangle Rectangle::boundingBoxImpl(double scale, double rotation) const {
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
    return Rectangle(std::max(x) - std::min(x), std::max(y) - std::min(y));
}

////////////////////////////////////////////////////////////////////////////////

// public

Square::Square(double side) : Rectangle(side, side) { }

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

double Ellipse::areaImpl(double scale) const {
    return std::numbers::pi * (scale * a_) * (scale * b_);
}

double Ellipse::boundingRadiusImpl(double scale) const { return scale * a_; }

Rectangle Ellipse::boundingBoxImpl(double scale, double rotation) const {
    return Rectangle(
        2 * scale * radius(rotation),
        2 * scale * radius(rotation + std::numbers::pi / 2));
}

// private const

double Ellipse::radius(double angle) const {
    return b_ / std::sqrt(1.0 - std::pow(e_ * std::cos(angle), 2));
}

////////////////////////////////////////////////////////////////////////////////

// public

Circle::Circle(double radius) : Ellipse(2 * radius, 2 * radius) { }

// private const override

Rectangle Circle::boundingBoxImpl(double, double) const {
    return Square(2 * boundingRadius());
}