// See [constexpr specifier](https://en.cppreference.com/w/cpp/language/constexpr)
// See [static members](https://en.cppreference.com/w/cpp/language/static)
// See [ODR-use](https://en.cppreference.com/w/cpp/language/definition#ODR-use)

class Const {
public:
  Const() : uninitialized_ {1} {}
  const int uninitialized_;   // must be initialized in constructor
  const int initialized_ = 2; // may be initialized in constructor
};

class StaticConst {
public:
  static const int uninitialized_; // initialized later
  static const int initialized_ = 2;
  static const int unused_ = 3;
  static const int dependent_ = StaticConst2::dependent_ + 1;
};

class StaticConst2 {
public:
  static const int dependent_ = StaticConst::initialized_ + 1;
  static const int dependent2_ = StaticConst::dependent_ + 1;
};

// a definition outside the class is required if odr-used
const int StaticConst::uninitialized_ = 1;
const int StaticConst::initialized_; // already initialized
// const int StaticConst::unused_ ; // already initialized

class ConstExpr {
public:
  // constexpr int non_static_ = 0; // must be static
  // static constexpr int uninitialized_; // must be initialized
  static constexpr int initialized_ = 2;
};

// a definition outside the class is optional
// constexpr int ConstExpr::initialized_; // already initialized

////////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

TEST(Const, const) {
  Const instance;
  EXPECT_EQ(1, instance.uninitialized_);
  EXPECT_EQ(2, instance.initialized_);
}

TEST(Const, static_const) {
  EXPECT_EQ(1, StaticConst::uninitialized_);
  EXPECT_EQ(2, StaticConst::initialized_);
}

TEST(Const, static_dependent) {
  EXPECT_EQ(5, StaticConst2::dependent2_);
}

TEST(Const, constexpr) { EXPECT_EQ(2, ConstExpr::initialized_); }
