#include "Gadget.hpp"
#include "compat/gsl14.hpp"

Gadget::Gadget(Protected p) : Widget { p, gsl::czstring(__func__) } { }

Gadget::Gadget(Protected, Gadget&& other) : Gadget { std::move(other) } { }

Gadget::Gadget(Protected, const Gadget& other) : Gadget { other } { }

Gadget::Gadget(Protected p, const std::string& name) : Widget { p, name } { }
