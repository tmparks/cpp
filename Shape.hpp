#pragma once

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

class Shape {
public:
    virtual ~Shape() = default;
    double area() const;               // Area.
    Circle boundingCircle() const;     // Bounding circle.
    Rectangle boundingBox() const;     // Bounding rectangle.
    void scaleTo(double s);            // Set scale.
    void scaleBy(double ds);           // Adjust scale.
    void rotateTo(double angle);       // Set rotation.
    void rotateBy(double dangle);      // Adjust rotation.
    void moveTo(double x, double y);   // Set position.
    void moveBy(double dx, double dy); // Adjust position.

private:
    // Not affected by rotation or position.
    virtual double areaImpl(double scale) const = 0;

    // Not affected by rotation or position.
    virtual double boundingRadiusImpl(double scale) const = 0;

    // Does not take position into account.
    // Returns width and height.
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
    double areaImpl(double scale) const override;
    double boundingRadiusImpl(double scale) const override;
    std::tuple<double, double> boundingBoxImpl(
            double scale, double rotation) const override;
    const double width_ { 0.0 };  // Initial width.
    const double height_ { 0.0 }; // Initial height.
};

////////////////////////////////////////////////////////////////////////////////

class Square : public Rectangle {
public:
    Square(double side);
};

////////////////////////////////////////////////////////////////////////////////

class Ellipse : public Shape {
public:
    Ellipse(double width, double height);

private:
    double areaImpl(double scale) const override;
    double boundingRadiusImpl(double scale) const override;
    std::tuple<double, double> boundingBoxImpl(
            double scale, double rotation) const override;
    double radius(double angle) const;
    const double a_; // Initial length of semi-major axis.
    const double b_; // Initial length of semi-minor axis.
    const double e_; // Eccentricity.
};

////////////////////////////////////////////////////////////////////////////////

class Circle : public Ellipse {
public:
    Circle(double radius);
};
