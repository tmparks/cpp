#pragma once

#include <memory>
#include <tuple>

////////////////////////////////////////////////////////////////////////////////

class Point {
public:
    static Point rectangular(double x, double y);
    static Point polar(double radius, double angle);
    double x() const;
    double y() const;
    double radius() const;
    double angle() const;

private:
    Point(double x, double y);
    double x_ { 0.0 };
    double y_ { 0.0 };
};

double distance(const Point& a, const Point& b);

////////////////////////////////////////////////////////////////////////////////

// Forward declarations.
class Circle;
class Rectangle;

// See [What is a "virtual constructor"?](https://isocpp.org/wiki/faq/virtual-functions#virtual-ctors)
// See [Non-virtual interface pattern](https://en.wikipedia.org/wiki/Non-virtual_interface_pattern)
class Shape {
public:
    virtual ~Shape() = default;
    std::unique_ptr<Shape> clone() const; // Create a copy.
    double area() const;                  // Area.
    Circle boundingCircle() const;        // Bounding circle.
    Rectangle boundingBox() const;        // Bounding rectangle.
    Shape& scaleTo(double s);             // Set scale.
    Shape& scaleBy(double ds);            // Adjust scale.
    Shape& rotateTo(double radians);      // Set rotation angle.
    Shape& rotateBy(double dradians);     // Adjust rotation angle.
    Shape& moveTo(double x, double y);    // Set position.
    Shape& moveBy(double dx, double dy);  // Adjust position.

protected:
    Shape() = default;
    Shape(const Shape&) = default;
    Shape(Shape&&) = default;
    Shape& operator=(const Shape&) = default;
    Shape& operator=(Shape&&) = default;

private:
    // By convention, derived classes implement a virtual constructor
    // with a covariant return type, which is not possible with smart pointers.
    virtual Shape* cloneImpl() const = 0;

    // Not affected by rotation or position.
    virtual double areaImpl(double scale) const = 0;

    // Not affected by rotation or position.
    virtual double boundingRadiusImpl(double scale) const = 0;

    // Not affected by position. Returns width and height.
    virtual std::tuple<double, double> boundingBoxImpl(
            double scale, double rotation) const = 0;

    double scale_ { 1.0 };
    double rotation_ { 0.0 };
    Point position_ { Point::rectangular(0.0, 0.0) };
};

////////////////////////////////////////////////////////////////////////////////

class Rectangle : public Shape {
public:
    Rectangle(double width, double height);

private:
    Rectangle* cloneImpl() const override;
    double areaImpl(double scale) const override;
    double boundingRadiusImpl(double scale) const override;
    std::tuple<double, double> boundingBoxImpl(
            double scale, double rotation) const override;
    double width_ { 0.0 };  // Initial width.
    double height_ { 0.0 }; // Initial height.
};

////////////////////////////////////////////////////////////////////////////////

class Square : public Rectangle {
public:
    Square(double side);

private:
    Square* cloneImpl() const override;
};

////////////////////////////////////////////////////////////////////////////////

class Ellipse : public Shape {
public:
    Ellipse(double width, double height);

private:
    Ellipse* cloneImpl() const override;
    double areaImpl(double scale) const override;
    double boundingRadiusImpl(double scale) const override;
    std::tuple<double, double> boundingBoxImpl(
            double scale, double rotation) const override;
    double radius(double angle) const;
    double a_; // Initial length of semi-major axis.
    double b_; // Initial length of semi-minor axis.
    double e_; // Eccentricity.
};

////////////////////////////////////////////////////////////////////////////////

class Circle : public Ellipse {
public:
    Circle(double radius);

private:
    Circle* cloneImpl() const override;
};
