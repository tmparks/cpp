#include "Widget.hpp"
#include "compat/gsl14.hpp"
#include <gtest/gtest.h>

void share_const(const Widget& w) {
    auto p = w.shared_from_this();
    std::cout << gsl::czstring(__func__) << ": use_count=" << p.use_count()
              << std::endl;
}

void share(Widget& w) {
    auto p = w.shared_from_this();
    std::cout << gsl::czstring(__func__) << ": use_count=" << p.use_count()
              << std::endl;
}

TEST(SharedFromThis, clone) {
    auto p1 = create<Widget>();
    auto p2 = p1->clone();
}

TEST(SharedFromThis, share) {
    auto p = create<Widget>();
    share(*p);
    share_const(*p);
}
