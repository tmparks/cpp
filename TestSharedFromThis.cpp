#include "SharedObject.hpp"
#include "Verbose.hpp"
#include "compat/gsl14.hpp"
#include <gtest/gtest.h>

// Derived publicly (but indirectly) from std::enable_shared_from_this
class Widget : public Verbose<SharedObject> {
public:
    ~Widget() noexcept override = default;
    Widget& operator=(const Widget& other) = default;
    Widget& operator=(Widget&& other) noexcept = default;

public: // pseudo-protected
    explicit Widget(Protected) noexcept : Widget{} { }
    Widget(Protected, const Widget& other) : Widget{other} { }
    Widget(Protected, Widget&& other) noexcept : Widget{std::move(other)} { }
    Widget(Protected, const std::string& name) : Verbose<SharedObject>{name} { }

protected:
    Widget() noexcept : Verbose<SharedObject>{gsl::czstring{__func__}} { }
    Widget(const Widget& other) = default;
    Widget(Widget&& other) noexcept = default;
};

// Derived publicly (but indirectly) from std::enable_shared_from_this
class Gadget : public Widget {
public:
    ~Gadget() noexcept override = default;
    Gadget& operator=(const Gadget& other) = default;
    Gadget& operator=(Gadget&& other) noexcept = default;

public: // pseudo-protected
    explicit Gadget(Protected) noexcept : Gadget{} { }
    Gadget(Protected, const Gadget& other) : Gadget{other} { }
    Gadget(Protected, Gadget&& other) noexcept : Gadget{std::move(other)} { }
    Gadget(Protected tag, const std::string& name) : Widget{tag, name} { }

protected:
    Gadget() noexcept : Widget{Protected{}, gsl::czstring{__func__}} { }
    Gadget(const Gadget& other) = default;
    Gadget(Gadget&& other) noexcept = default;
};

void share_const(const Widget& w) {
    auto p = w.shared_from_this();
    std::cout << gsl::czstring{__func__} << ": use_count=" << p.use_count()
              << std::endl;
}

void share(Widget& w) {
    auto p = w.shared_from_this();
    std::cout << gsl::czstring{__func__} << ": use_count=" << p.use_count()
              << std::endl;
}

TEST(SharedFromThis, create) {
    auto w1 = create<Widget>();
    auto g2 = create<Gadget>();
}

TEST(SharedFromThis, copy) {
    auto w1 = create<Widget>();
    auto w2 = create<Widget>(*w1); // copy construction
    auto w3 = create<Widget>("Widget three");
    *w3 = *w1; // copy assignment
    auto g1 = create<Gadget>();
    auto g2 = create<Gadget>(*g1); // copy construction
    auto g3 = create<Gadget>("Gadget three");
    *g3 = *g1; // copy assignment
}

TEST(SharedFromThis, move) {
    auto w1 = create<Widget>();
    auto w2 = create<Widget>(std::move(*w1)); // move construction
    auto w3 = create<Widget>("Widget three");
    auto w4 = create<Widget>("Widget four");
    *w4 = std::move(*w3); // move assignment
    auto g1 = create<Gadget>();
    auto g2 = create<Gadget>(std::move(*g1)); // move construction
    auto g3 = create<Gadget>("Gadget three");
    auto g4 = create<Gadget>("Gadget four");
    *g4 = std::move(*g3); // move assignment
}

TEST(SharedFromThis, share) {
    auto w1 = create<Widget>();
    share(*w1);
    share_const(*w1);
    auto g1 = create<Gadget>();
    share(*g1);
    share_const(*g1);
}
