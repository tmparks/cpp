#pragma once
#include "Widget.hpp"

// Derived publicly (and indirectly) from std::enable_shared_from_this
class Gadget : public Widget {
public: // pseudo-protected
    explicit Gadget(Protected);
    Gadget(Protected, const Gadget& w);

protected:
    Gadget(const std::string& name);

private:
    std::shared_ptr<AbstractWidget> cloneImpl() const override;
};
