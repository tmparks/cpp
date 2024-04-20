#pragma once
#include "Widget.hpp"

// Derived publicly (but indirectly) from std::enable_shared_from_this
class Gadget : public Widget {
public: // pseudo-protected
    explicit Gadget(Protected);
    Gadget(Protected, Gadget&& other);
    Gadget(Protected, const Gadget& other);
    Gadget(Protected, const std::string& name);
};
