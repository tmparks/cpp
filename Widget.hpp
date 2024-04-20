#pragma once
#include "SharedObject.hpp"
#include "Verbose.hpp"

// Derived publicly (but indirectly) from std::enable_shared_from_this
class Widget : public SharedObject {
public: // pseudo-protected
    explicit Widget(Protected);
    Widget(Protected, Widget&& other);
    Widget(Protected, const Widget& other);
    Widget(Protected, const std::string& name);

protected:
    Verbose v_;
};
