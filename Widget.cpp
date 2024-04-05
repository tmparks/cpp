#include "Widget.hpp"
#include "compat/gsl14.hpp"

Widget::Widget(Protected) : Widget { gsl::czstring(__func__) } { }

Widget::Widget(Protected, const Widget& w) : Widget { w } { }

Widget::Widget(const std::string& name) : AbstractWidget { name } { }

std::shared_ptr<AbstractWidget> Widget::cloneImpl() const {
    return std::make_shared<Widget>(Protected {}, *this);
}
