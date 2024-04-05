#pragma once
#include "AbstractWidget.hpp"

// Derived publicly (and indirectly) from std::enable_shared_from_this
class Widget : public AbstractWidget {
public:
    ~Widget() override = default;

public: // pseudo-protected
    explicit Widget(Protected);
    Widget(Protected, const Widget& w);

protected:
    Widget(const std::string& name);
    Widget(const Widget&) = default;

    // Rule of five.
    Widget() = delete;
    Widget(Widget&&) = delete;
    Widget& operator=(const Widget&) = delete;
    Widget& operator=(Widget&&) = delete;

private:
    std::shared_ptr<AbstractWidget> cloneImpl() const override;
};
