#pragma once
#include "AbstractWidget.hpp"

// Derived publicly (and indirectly) from std::enable_shared_from_this
class Widget : public AbstractWidget {
public: // pseudo-protected
    explicit Widget(Protected);
    Widget(Protected, const Widget& w);

protected:
    Widget(const std::string& name);

private:
    std::shared_ptr<AbstractWidget> cloneImpl() const override;
};
