#include "StrongOperators.hpp"
#include "StrongType.hpp"

#include <gtest/gtest.h>

// Use opaque scoped enumerations for strong identifier types.
enum class RedId : uintmax_t { invalid = 0 };
enum class BlueId : uintmax_t { invalid = 0 };

// Generate the next ID.
template <typename E>
E next() {
    // Hide unsightly casts inside a function.
    static auto previous = static_cast<std::underlying_type<E>::type>(E::invalid);
    return static_cast<E>(++previous);
}

// Use simple struct for a generic strong type.
struct Length : StrongType<double> { };
struct Width : Length { };
struct Height : Length { };
struct Area : StrongType<double> { };

STRONG_FLOATING_POINT_OPERATORS(Length)

TEST(StrongType, id) {
    auto red = RedId{1};
    auto rojo = RedId{1};
    auto rouge = RedId::invalid;
    auto blue = BlueId{2};
    auto azul = BlueId{2};
    auto bleu = BlueId::invalid;

    EXPECT_EQ(red, rojo);
    EXPECT_EQ(blue, azul);

    EXPECT_NE(red, RedId::invalid);
    EXPECT_NE(rojo, RedId::invalid);
    EXPECT_EQ(rouge, RedId::invalid);

    EXPECT_NE(blue, BlueId::invalid);
    EXPECT_NE(azul, BlueId::invalid);
    EXPECT_EQ(bleu, BlueId::invalid);
    // EXPECT_EQ(bleu, invalid); // compile time error

    EXPECT_EQ(red, next<RedId>()); // 1
    EXPECT_NE(red, next<RedId>()); // 2

    EXPECT_NE(blue, next<BlueId>()); // 1
    EXPECT_EQ(blue, next<BlueId>()); // 2

    // Cannot compare different types.
    // EXPECT_NE(RedId::invalid, BlueId::invalid); // compile time error
    // EXPECT_NE(red, blue); // compile time error
    // EXPECT_NE(blue, other); // compile time error
}

TEST(StrongType, initialization) {
    // Strong types can be initialized the same way as their underlying types.
    auto w = Width{3.0};
    auto h = Height{4.0};
    auto area = Area{Length{w} * Length{h}};
    EXPECT_EQ(Area{12.0}, area);
}
