#include "StrongOperators.hpp"
#include "StrongType.hpp"

#include <gtest/gtest.h>

// Good: Use simple struct for strong type, comment expresses intent.
// Use simple [aggregate initialization](https://en.cppreference.com/w/cpp/language/aggregate_initialization)
struct Mass { double asDouble{0.0}; }; // Strong type.

// Better: Use inheritance so that code expresses intent.
// [Express ideas directly in code](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#p1-express-ideas-directly-in-code)
#if __cplusplus >= 201703L
struct Length : StrongType<double> { };
struct Width : Length { };
struct Height : Length { };
struct Area : StrongType<double> { };
#else  // C++17
struct Length : StrongType<double> { using base::base; };
struct Width : Length { using Length::Length; };
struct Height : Length { using Length::Length; };
struct Area : StrongType<double> { using base::base; };
#endif // C++17

////////////////////////////////////////////////////////////////////////////////

// Bad: Use opaque scoped enum for strong identifier type, comment expresses intent.
enum class RedId : uintmax_t { null = 0 }; // Strong identifier.

// Bad: Use MACRO because scoped enum does not allow inheritance.
// [Don’t use macros for constants or “functions”](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#es31-dont-use-macros-for-constants-or-functions)
// NOLINTNEXTLINE(*-macro-usage)
#define STRONG_IDENTIFIER(type) enum class type : uintmax_t { null = 0 }

STRONG_IDENTIFIER(BlueId);

// Generate the next ID.
template <typename E>
typename std::enable_if<std::is_enum<E>::value, E>::type next() {
    static auto previous =
            static_cast<typename std::underlying_type<E>::type>(E::null);
    return static_cast<E>(++previous);
}

// Better: Use inheritance so that code expresses intent.
// [Express ideas directly in code](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#p1-express-ideas-directly-in-code)
#if __cplusplus >= 201703L
struct GreenId : StrongIdentifier<> { };
#else  // C++17
struct GreenId : StrongIdentifier<> { using base::base; };
#endif // C++17

// Generate the next ID.
template <typename T>
typename std::enable_if<not std::is_enum<T>::value, T>::type next() {
    static auto previous = T::null;
    return T{++(previous.value)};
}

STRONG_FLOATING_POINT_OPERATORS(Length)

TEST(StrongType, initialization) {
    auto m = Mass{2.0}; // NOLINT(*-avoid-magic-numbers)
    EXPECT_EQ(2.0, m.asDouble);

    auto w = Width{3.0};  // NOLINT(*-avoid-magic-numbers)
    auto h = Height{4.0}; // NOLINT(*-avoid-magic-numbers)
    auto area = Area{Length{w} * Length{h}};
    EXPECT_EQ(Area{12.0}, area);
}

TEST(StrongIdentifier, initialization) {
    GreenId zero{0};
    GreenId default1;
    GreenId default2{};

    EXPECT_EQ(zero, GreenId::null);
    EXPECT_EQ(default1, GreenId::null);
    EXPECT_EQ(default2, GreenId::null);
    EXPECT_EQ(default1, default2);
}

TEST(StrongIdentifier, equal) {
#if __cplusplus >= 201703L
    auto red = RedId{1};
    auto rojo = RedId{1};
    auto rouge = RedId::null;
    auto blue = BlueId{2};
    auto azul = BlueId{2};
    auto bleu = BlueId::null;
#else  // C++17
    auto red = static_cast<RedId>(1);
    auto rojo = static_cast<RedId>(1);
    auto rouge = RedId::null;
    auto blue = static_cast<BlueId>(2);
    auto azul = static_cast<BlueId>(2);
    auto bleu = BlueId::null;
#endif // C++17

    EXPECT_EQ(red, rojo);
    EXPECT_EQ(blue, azul);

    EXPECT_NE(red, RedId::null);
    EXPECT_NE(rojo, RedId::null);
    EXPECT_EQ(rouge, RedId::null);

    EXPECT_NE(blue, BlueId::null);
    EXPECT_NE(azul, BlueId::null);
    EXPECT_EQ(bleu, BlueId::null);

    EXPECT_EQ(red, next<RedId>()); // 1
    EXPECT_NE(red, next<RedId>()); // 2

    EXPECT_NE(blue, next<BlueId>()); // 1
    EXPECT_EQ(blue, next<BlueId>()); // 2

    // Cannot compare different types.
    // EXPECT_EQ(RedId::null, BlueId::null); // compile time error
    // EXPECT_NE(red, blue); // compile time error
    // EXPECT_EQ(blue, 2); // compile time error
}

TEST(StrongIdentifier, container) {
    auto collection = std::set<GreenId>{};
    EXPECT_EQ(0, collection.size());

    collection.insert(next<GreenId>()); // 1
    collection.insert(next<GreenId>()); // 2
    collection.insert(next<GreenId>()); // 3
    EXPECT_EQ(3, collection.size());

    collection.insert(GreenId{3});   // duplicate
    EXPECT_EQ(3, collection.size()); // size unchanged

    EXPECT_EQ(1, collection.erase(GreenId{1})); // remove smallest
    EXPECT_EQ(2, collection.cbegin()->value);   // new smallest
}
