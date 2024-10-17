#include "SharedCloneable.hpp"

#include "Verbose.hpp"
#include "compat/gsl14.hpp"
#include <gtest/gtest.h>
#include <vector>

// SharedCloneable enforces non-copyability.
// NOLINTNEXTLINE *-special-member-functions
class ConcreteObject : public SharedCloneable<ConcreteObject> {
public: // pseudo-protected
    ConcreteObject(Protected) : ConcreteObject{} { }
    ConcreteObject(Protected, const ConcreteObject& other) :
            ConcreteObject{other} { }

protected:
    ConcreteObject() = default;
    ConcreteObject(const ConcreteObject&) = default;
};

// SharedCloneable enforces non-copyability.
// NOLINTNEXTLINE *-special-member-functions
class DerivedObject : public SharedCloneable<DerivedObject, ConcreteObject> {
public: // pseudo-protected
    DerivedObject(Protected) : DerivedObject{} { }
    DerivedObject(Protected, const DerivedObject& other) :
            DerivedObject{other} { }

protected:
    DerivedObject() = default;
    DerivedObject(const DerivedObject&) = default;
};

TEST(SharedCloneable, clone) {
    auto p1 = make_verbose("concrete", create<ConcreteObject>());
    auto p2 = p1->shared_from_this(); // copy of pointer
    auto p3 = p1->clone();
    auto p4 = make_verbose("derived", create<DerivedObject>());
    auto p5 = p4->clone();
}
