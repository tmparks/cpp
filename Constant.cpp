// See [constexpr specifier](https://en.cppreference.com/w/cpp/language/constexpr)
// See [static members](https://en.cppreference.com/w/cpp/language/static)
// See [ODR-use](https://en.cppreference.com/w/cpp/language/definition#ODR-use)

// NOLINTBEGIN(*-avoid-const-or-ref-data-members)

class Const {
public:
  Const() : uninitialized_ {1} {}
  const int uninitialized_;   // must be initialized in constructor
  const int initialized_ = 2; // may be initialized in constructor
};

// NOLINTEND(*-avoid-const-or-ref-data-members)

class StaticConst {
public:
  static const int uninitialized_; // initialized later
  static const int initialized_ = 4;
  static const int unused_ = 5;
};

// a definition outside the class is required if odr-used
const int StaticConst::uninitialized_ = 3;
const int StaticConst::initialized_; // already initialized
// const int StaticConst::unused_ ; // already initialized

// must be initialized after the constant upon which it depends
static const int dependent_non_member = StaticConst::uninitialized_ + 100;

class StaticConst2 {
public:
  static const int dependent_ = StaticConst::initialized_ + 100;
  static const int dependent2_ = dependent_non_member + 100;
};

// a definition outside the class is required if odr-used
const int StaticConst2::dependent_;
const int StaticConst2::dependent2_;

#if __cplusplus >= 201703L

class ConstExpr {
public:
  // constexpr int non_static_ = 0; // must be static
  // static constexpr int uninitialized_; // must be initialized
  static constexpr int initialized_ = 2;
};

#endif // C++17

// a definition outside the class is optional
// constexpr int ConstExpr::initialized_; // already initialized

////////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>
#include <vector>

TEST(Const, const) {
  Const instance;
  EXPECT_EQ(1, instance.uninitialized_);
  EXPECT_EQ(2, instance.initialized_);
}

TEST(Const, static_const) {
  EXPECT_EQ(3, StaticConst::uninitialized_);
  EXPECT_EQ(4, StaticConst::initialized_);
}

TEST(Const, static_dependent) {
  EXPECT_EQ(104, StaticConst2::dependent_);
  EXPECT_EQ(203, StaticConst2::dependent2_);
}

#if __cplusplus >= 201703L

TEST(Const, constexpr) { EXPECT_EQ(2, ConstExpr::initialized_); }

TEST(Const, constexpr_odr_used) {
  std::vector<int> v;
  v.push_back(ConstExpr::initialized_);
  const auto & ref = ConstExpr::initialized_;
  EXPECT_EQ(2, ref);
}

#endif // C++17
