#include "Verbose.hpp"
#include "compat/ranges23.hpp"
#include <gtest/gtest.h>
#include <tuple>

class Zip : public testing::Test {
public:
    void SetUp() override {
        // NOLINTBEGIN(*-avoid-magic-numbers)
        left = { 1, 2, 3, 4, 5 };
        right = { 1.1, 2.2, 3.3, 4.4, 5.5 };

        // Carefully avoid copies and moves.
        center.reserve(5);
        for (const auto& name : { "one", "two", "three", "four", "five" }) {
            center.emplace_back(name);
        }
        // NOLINTEND(*-avoid-magic-numbers)
    }

    template <typename T>
    void print(const std::string& label, const T& range) {
        std::cout << label;
        for (const auto& element : range) {
            std::cout << element << " ";
        }
        std::cout << std::endl;
    }

    std::vector<int> left;
    std::vector<Verbose> center;
    std::vector<double> right;
};

TEST_F(Zip, print) {
    print("left: ", left);
    print("center: ", center);
    print("right: ", right);
    print("zip(left,center,right): ",
          std::ranges::views::zip(left, center, right));
}

#if __cplusplus >= 201703L

TEST_F(Zip, for_mutable) {
    print("before: ", std::ranges::views::zip(left, center, right));

    // When iterating over a single sequence, convention dictates that we use
    // a non-const lvalue reference. The presence of the reference makes it
    // clear that no copy is being made. The absence of the const keyword makes
    // it clear that the elements can be modified.
    for (auto& a : center) {
        a.name().append("?");
    }

    // Because each element of a zipped sequence is a temporary tuple,
    // we cannot use a non-const lvalue reference in the for loop, but we can
    // safely make a copy. Because the tuple contains references, we are not
    // making copies of the underlying sequence elements themselves. However,
    // the absence of a reference may be confusing. It looks like we are making
    // copies and that any modifications affect only the copies, which would
    // probably be a bug.
    for (auto [a, b, c] : std::ranges::views::zip(left, center, right)) {
        // Verify previous modifications.
        EXPECT_EQ('?', b.name().back());
        // Make additional modifications.
        c -= a++;
        b.name().append("!");
    }

    // We could use a const lvalue reference, but the presence of the const
    // keyword may be confusing. It looks like we should not be able to modify
    // the sequence elements.
    for (const auto& [a, b, c] : std::ranges::views::zip(left, center, right)) {
        // Verify previous modifications.
        EXPECT_FLOAT_EQ(0.1 * (a - 1), c);
        EXPECT_EQ('!', b.name().back());
        // Make additional modifications.
        c = 1.1 * a++; // NOLINT(*-avoid-magic-numbers)
        b.name().pop_back();
    }

    // The least confusing solution uses a universal reference.
    // The presence of the reference makes it clear that no copy is being made.
    // The absence of the const keyword makes it clear that the elements can be
    // modified.
    for (auto&& [a, b, c] : std::ranges::views::zip(left, center, right)) {
        // Verify previous modifications.
        EXPECT_FLOAT_EQ(1.1 * (a - 1), c);
        EXPECT_EQ('?', b.name().back());
        // Make additional modifications.
        a -= 2;
        c -= 1.1; // NOLINT(*-avoid-magic-numbers)
        b.name().pop_back();
    }

    // Nothing stops us from using a universal reference when iterating over a
    // single sequence, though it may look unconventional.
    for (auto&& a : center) {
        a.name().append("*");
        a.name().pop_back();
    }

    print("after: ", std::ranges::views::zip(left, center, right));
}

#endif // C++17

TEST_F(Zip, for_mutable_tuple) {
    print("before: ", std::ranges::views::zip(left, center, right));

    // We can use a tuple instead of structured binding. We cannot use a
    // non-const lvalue reference, so we use a universal reference to
    // minimize confusion. (See discussion above.)
    for (auto&& tuple : std::ranges::views::zip(left, center, right)) {
        auto& a = std::get<0>(tuple);
        auto& b = std::get<1>(tuple);
        auto& c = std::get<2>(tuple);
        c -= a++;
        b.name().append("!");
    }
    for (auto&& tuple : std::ranges::views::zip(left, center, right)) {
        auto& a = std::get<0>(tuple);
        auto& b = std::get<1>(tuple);
        auto& c = std::get<2>(tuple);
        // Verify previous modifications.
        EXPECT_FLOAT_EQ(0.1 * (a - 1), c);
        EXPECT_EQ('!', b.name().back());
        // Put things back the way we found them.
        c += --a;
        b.name().pop_back();
    }

    print("after: ", std::ranges::views::zip(left, center, right));
}

TEST_F(Zip, for_const) {
    // When iterating over a single sequence, convention dictates that we use
    // a const lvalue reference when we do not intend to modify the elements.
    // The presence of the reference makes it clear that no copy is being made.
    // The presence of the const keyword makes it clear that the elements cannot
    // be modified.
    for (const auto& a : center) {
        // a.name().append("!"); // compile time error
        EXPECT_NE('!', a.name().back());
    }

    // When the sequence itself is const, the const keyword becomes optional,
    // but we can use it anyway to make our intentions clear. This also guards
    // against future changes in the declaration of the sequence.
    const auto& ccenter = center;
    for (const auto& a : ccenter) {
        // a.name().append("!"); // compile time error
        EXPECT_NE('!', a.name().back());
    }

#if __cplusplus >= 201703L

    // To make our intentions more obvious, we can explicitly make the sequence const.
    for (const auto& a : std::as_const(center)) {
        // a.name().append("!"); // compile time error
        EXPECT_NE('!', a.name().back());
    }

    // Unfortunately using a const lvalue reference does not have the intended
    // effect when iterating over a zipped sequence. This is because each tuple
    // in the zipped sequence contains non-const references.
    // The tuple may be const, but the elements can still be modified!
    for (const auto& [a, b, c] : std::ranges::views::zip(left, center, right)) {
        c -= a++;             // surprise!
        b.name().append("!"); // surprise!
    }

    // Instead, we must ensure that the underlying sequences being zipped are
    // const. The const keyword becomes optional, but we can use it anyway
    // to make our intentions clear.
    const auto& cleft = left;
    const auto& cright = right;
    for (const auto& [a, b, c] : std::ranges::views::zip(cleft, ccenter, cright)) {
        EXPECT_EQ('!', b.name().back());
        // c -= a++;             // compile time error
        // b.name().append("!"); // compile time error
    }

    // To make our intentions more obvious, and to guard against future changes
    // in the declarations of the sequences, we can explicitly make each
    // sequence const before combining them.
    for (const auto& [a, b, c] : std::ranges::views::zip(
                 std::as_const(left), std::as_const(center), std::as_const(right))) {
        EXPECT_EQ('!', b.name().back());
        // c -= a++;             // compile time error
        // b.name().append("!"); // compile time error
    }

#endif // C++17

#if __cplusplus >= 202302L

    // To make our intentions more obvious, and to guard against future changes
    // in the declarations of the sequences, we can explicitly make each
    // sequence const before combining them.
    for (const auto& [a, b, c] : std::ranges::views::zip(left, center, right) | std::views::as_const) {
        EXPECT_EQ('!', b.name().back());
        // c -= a++;             // compile time error
        // b.name().append("!"); // compile time error
    }

#endif // C++23
}

TEST_F(Zip, for_const_tuple) {
    // We can use a tuple instead of structured binding. The const keyword is
    // optional as it has no effect. This is because each tuple in the zipped
    // sequence contains non-const references. The tuple may be const, but the
    // elements could still be modified! Using const lvalue references as we
    // unpack the tuple makes it clear that the elements cannot be modified.
    for (const auto& tuple : std::ranges::views::zip(left, center, right)) {
        const auto& a = std::get<0>(tuple);
        const auto& b = std::get<1>(tuple);
        const auto& c = std::get<2>(tuple);
        EXPECT_FLOAT_EQ(1.1 * a, c);
        EXPECT_NE('!', b.name().back());
        // b.name().append("!"); // compile time error
    }
}

TEST_F(Zip, for_copy) {
    // When iterating over a single sequence, convention dictates that we
    // avoid using a reference when we intend to make copies of the elements.
    // The absence of a reference indicates that a copy is being made.
    // The absence of the const keyword indicates that the copies can be modified.
    for (auto a : center) {
        a.name().append("!");
        EXPECT_EQ('!', a.name().back()); // Verify that copy is modified.
    }
    for (const auto& a : center) {
        EXPECT_NE('!', a.name().back()); // Verify that original is not modified.
    }

#if __cplusplus >= 201703L

    // Unfortunately using the same convention with a zipped sequence does not
    // have the intended effect. This is because each tuple in the zipped
    // sequence contains non-const references. The tuple may have been copied,
    // but the original elements can still be modified!
    for (auto [a, b, c] : std::ranges::views::zip(left, center, right)) {
        c -= a++;             // surprise!
        b.name().append("!"); // surprise!
        // Put things back the way we found them.
        c += --a;
        b.name().pop_back();
    }

#endif // C++17

    // The least confusing solution is to make copies as we unpack a tuple.
    for (auto tuple : std::ranges::views::zip(left, center, right)) {
        auto a = std::get<0>(tuple);
        auto b = std::get<1>(tuple);
        auto c = std::get<2>(tuple);
        EXPECT_FALSE(std::is_reference<decltype(a)>::value);
        EXPECT_FALSE(std::is_reference<decltype(b)>::value);
        EXPECT_FALSE(std::is_reference<decltype(c)>::value);
        // Modify the copies.
        c -= a++; // NOLINT(*-deadcode.DeadStores)
        b.name().append("!");
    }
    for (const auto& tuple : std::ranges::views::zip(left, center, right)) {
        const auto& a = std::get<0>(tuple);
        const auto& b = std::get<1>(tuple);
        const auto& c = std::get<2>(tuple);
        // Verify that original values were not modified.
        EXPECT_FLOAT_EQ(1.1 * a, c);
        EXPECT_NE('!', b.name().back());
    }
}
