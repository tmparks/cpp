#include "SharedCloneable.hpp"

#include "Verbose.hpp"
#include "compat/gsl14.hpp"
#include <gtest/gtest.h>
#include <vector>

class ConcreteObject : public SharedCloneable<ConcreteObject> {
public:
    ~ConcreteObject() override = default;

public: // pseudo-protected
    ConcreteObject(Protected) : v_ { gsl::czstring(__func__) } { }
    ConcreteObject(Protected, const ConcreteObject& other) :
            ConcreteObject { other } { }

protected:
    ConcreteObject() = default;
    ConcreteObject(ConcreteObject&&) = delete;
    ConcreteObject(const ConcreteObject&) = default;
    ConcreteObject& operator=(ConcreteObject&&) = delete;
    ConcreteObject& operator=(const ConcreteObject&) = delete;

private:
    Verbose v_ { "default" };
};

class DerivedObject : public SharedCloneable<DerivedObject, ConcreteObject> {
public:
    ~DerivedObject() override = default;

public: // pseudo-protected
    DerivedObject(Protected) : DerivedObject {} {}
    DerivedObject(Protected, const DerivedObject& other) : DerivedObject { other } {}

protected:
    DerivedObject() = default;
    DerivedObject(DerivedObject&&) = delete;
    DerivedObject(const DerivedObject&) = default;
    DerivedObject& operator=(DerivedObject&&) = delete;
    DerivedObject& operator=(const DerivedObject&) = delete;
};

TEST(SharedCloneable, clone) {
    auto p1 = create<ConcreteObject>();
    auto p2 = p1->shared_from_this(); // copy of pointer
    auto p3 = p1->clone();
    auto p4 = create<DerivedObject>();
    auto p5 = p4->clone();
    // auto o4 = *p3; // copy assignment
}
