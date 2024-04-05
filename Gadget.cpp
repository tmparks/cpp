#include "Gadget.hpp"
#include "compat/gsl14.hpp"

Gadget::Gadget(Protected) : Gadget { gsl::czstring(__func__) } { }

Gadget::Gadget(Protected, const Gadget& g) : Gadget { g } { }

Gadget::Gadget(const std::string& name) : Widget { name } { }

std::shared_ptr<AbstractWidget> Gadget::cloneImpl() const {
    return std::make_shared<Gadget>(Protected {}, *this);
}
