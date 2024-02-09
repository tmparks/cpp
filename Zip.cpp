#include "compat/ranges23.hpp"
#include <gtest/gtest.h>
#include <tuple>

class Zip : public testing::Test {
public:
    template <typename T>
    void print(const std::string& label, const T& range) {
        std::cout << label;
        for (const auto& element : range) {
            std::cout << element << " ";
        }
        std::cout << std::endl;
    }

    std::vector<int> left { 1, 2, 3, 4, 5 };
    std::vector<double> right { 1.1, 2.2, 3.3, 4.4, 5.5 };
};

TEST_F(Zip, print) {
    print("left: ", left);
    print("right: ", right);
    print("zip(left,right): ", std::ranges::views::zip(left, right));
}

#if __cpusplus >= 201703L

TEST_F(Zip, for_mutable) {
    print("before: ", right);
    for (auto&& [a, b] : std::ranges::views::zip(left, right)) {
        b -= a;
    }
    for (auto&& [a, b] : std::ranges::views::zip(left, right)) {
        EXPECT_FLOAT_EQ(0.1 * a, b);
    }
    print("after: ", right);
}

#endif // C++17

TEST_F(Zip, for_mutable_tuple) {
    print("before: ", right);
    for (auto tuple : std::ranges::views::zip(left, right)) {
        auto& a = std::get<0>(tuple);
        auto& b = std::get<1>(tuple);
        b -= a;
    }
    for (auto tuple : std::ranges::views::zip(left, right)) {
        auto& a = std::get<0>(tuple);
        auto& b = std::get<1>(tuple);
        EXPECT_FLOAT_EQ(0.1 * a, b);
    }
    print("after: ", right);
}

#if __cpusplus >= 201703L

TEST_F(Zip, for_const) {
    const auto& cleft = left;
    const auto& cright = right;
    for (auto [a, b] : std::ranges::views::zip(cleft, cright)) {
        EXPECT_FLOAT_EQ(1.1 * a, b);
        // b = 0.0; // error
    }
}

TEST_F(Zip, for_as_const) {
    for (auto [a, b] :
         std::ranges::views::zip(std::as_const(left), std::as_const(right))) {
        EXPECT_FLOAT_EQ(1.1 * a, b);
        // b = 0.0; // error
    }
}

#endif // C++17

TEST_F(Zip, for_const_tuple) {
    for (auto tuple : std::ranges::views::zip(left, right)) {
        const auto& a = std::get<0>(tuple);
        const auto& b = std::get<1>(tuple);
        EXPECT_FLOAT_EQ(1.1 * a, b);
        // b = 0.0; // error
    }
}
