#include "Widget.hpp"
#include "compat/gsl14.hpp"

Widget::Widget(Protected) : AbstractWidget(gsl::czstring(__func__)) { }

Widget::Widget(Protected, const Widget& w) : Widget(w) { }

std::shared_ptr<AbstractWidget> Widget::cloneImpl() const {
    return std::make_shared<Widget>(Protected {}, *this);
}
