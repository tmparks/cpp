#include "AbstractWidget.hpp"

std::shared_ptr<AbstractWidget> AbstractWidget::clone() const {
    return cloneImpl();
}

AbstractWidget::AbstractWidget(const std::string& name) : v_(name) { }
