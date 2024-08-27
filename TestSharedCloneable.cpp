#include "SharedCloneable.hpp"

#include "Verbose.hpp"
#include "compat/gsl14.hpp"
#include <gtest/gtest.h>
#include <vector>

// SharedCloneable enforces non-copyability.
// NOLINTNEXTLINE *-special-member-functions
class ConcreteObject : public SharedCloneable<ConcreteObject> {
public: // pseudo-protected
    ConcreteObject(Protected) :
            ConcreteObject { gsl::czstring { __func__ } } { }
    ConcreteObject(Protected, const ConcreteObject& other) :
            ConcreteObject { other } { }

protected:
    ConcreteObject(const std::string& name) : v_ { name } {};
    ConcreteObject(const ConcreteObject&) = default;

private:
    Verbose v_ { "default" };
};

// SharedCloneable enforces non-copyability.
// NOLINTNEXTLINE *-special-member-functions
class DerivedObject : public SharedCloneable<DerivedObject, ConcreteObject> {
public: // pseudo-protected
    DerivedObject(Protected) : DerivedObject { gsl::czstring { __func__ } } { }
    DerivedObject(Protected, const DerivedObject& other) :
            DerivedObject { other } { }

protected:
    DerivedObject(const std::string& name) : SharedCloneable { name } { }
    DerivedObject(const DerivedObject&) = default;
};

TEST(SharedCloneable, clone) {
    auto p1 = create<ConcreteObject>();
    auto p2 = p1->shared_from_this(); // copy of pointer
    auto p3 = p1->clone();
    auto p4 = create<DerivedObject>();
    auto p5 = p4->clone();
}
