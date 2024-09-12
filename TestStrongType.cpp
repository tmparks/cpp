#include "StrongOperators.hpp"
#include "StrongType.hpp"

#include <gtest/gtest.h>

// Good: Use simple struct for strong type, comment expresses intent.
struct Mass {
    double asDouble{0.0};
}; // Strong type.

// Better: Use inheritance so that code expresses intent.
// [Express ideas directly in code](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#p1-express-ideas-directly-in-code)
struct Length : StrongType<double> { };
struct Width : Length { };
struct Height : Length { };
struct Area : StrongType<double> { };

////////////////////////////////////////////////////////////////////////////////

// Bad: Use opaque scoped enum for strong identifier type, comment expresses intent.
enum class RedId : uintmax_t { null = 0 }; // Strong identifier.

// Bad: Use MACRO because scoped enum does not allow inheritance.
// [Don’t use macros for constants or “functions”](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#es31-dont-use-macros-for-constants-or-functions)
#define STRONG_IDENTIFIER(type) enum class type : uintmax_t { null = 0 }

STRONG_IDENTIFIER(BlueId);

// Generate the next ID.
template <typename E>
typename std::enable_if<std::is_enum<E>::value, E>::type next() {
    static auto previous = static_cast<std::underlying_type<E>::type>(E::null);
    return static_cast<E>(++previous);
}

// Better: Use inheritance so that code expresses intent.
// [Express ideas directly in code](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#p1-express-ideas-directly-in-code)
struct GreenId : StrongIdentifier<> { };

// Generate the next ID.
template <typename T>
typename std::enable_if<not std::is_enum<T>::value, T>::type next() {
    static auto previous = T::null;
    return {++previous};
}

STRONG_FLOATING_POINT_OPERATORS(Length)

TEST(StrongType, id) {
    auto red = RedId{1};
    auto rojo = RedId{1};
    auto rouge = RedId::null;
    auto blue = BlueId{2};
    auto azul = BlueId{2};
    auto bleu = BlueId::null;

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

TEST(StrongType, initialization) {
    // Strong types can be initialized the same way as their underlying types.
    auto w = Width{3.0};
    auto h = Height{4.0};
    auto area = Area{Length{w} * Length{h}};
    EXPECT_EQ(Area{12.0}, area);
}
