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
}

TEST(SharedFromThis, copy) {
    auto w1 = create<Widget>();
    auto w2 = create<Widget>(*w1); // copy construction
    auto w3 = create<Widget>("three");
    *w3 = *w1; // copy assignment
}

TEST(SharedFromThis, move) {
    auto g1 = create<Gadget>();
    auto g2 = create<Gadget>(std::move(*g1)); // move construction
    auto g3 = create<Gadget>("three");
    auto g4 = create<Gadget>("four");
    *g4 = std::move(*g3); // move assignment
}

TEST(SharedFromThis, share) {
    auto w1 = create<Widget>();
    share(*w1);
    share_const(*w1);
}
