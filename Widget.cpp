#include "Widget.hpp"
#include "compat/gsl14.hpp"

Widget::Widget(Protected) : v_ { gsl::czstring(__func__) } { }

Widget::Widget(Protected, Widget&& other) : Widget { std::move(other) } {}

Widget::Widget(Protected, const Widget& other) : Widget { other } { }

Widget::Widget(Protected, const std::string& name) : v_ { name } { }
