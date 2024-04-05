#include "Gadget.hpp"
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

TEST(SharedFromThis, create) {
    auto w1 = create<Widget>();
    auto g2 = create<Gadget>();
    // auto w3 = create<Widget>(*w1); // copy forbidden
    // auto g4 = create<Gadget>(*w1); // copy forbidden
    // auto g5 = create<Widget>(*g2); // copy forbidden
}

TEST(SharedFromThis, clone) {
    auto w1 = create<Widget>();
    auto w2 = w1->clone();
}

TEST(SharedFromThis, share) {
    auto w1 = create<Widget>();
    share(*w1);
    share_const(*w1);
}
