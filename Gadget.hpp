#pragma once
#include "Widget.hpp"

// Derived publicly (and indirectly) from std::enable_shared_from_this
class Gadget : public Widget {
public:
    ~Gadget() override = default;

public: // pseudo-protected
    explicit Gadget(Protected);
    Gadget(Protected, const Gadget& w);

protected:
    Gadget(const std::string& name);
    Gadget(const Gadget&) = default;

    // Rule of five.
    Gadget() = delete;
    Gadget(Gadget&&) = delete;
    Gadget& operator=(const Gadget&) = delete;
    Gadget& operator=(Gadget&&) = delete;

private:
    std::shared_ptr<AbstractWidget> cloneImpl() const override;
};
