#include <Eigen/Eigen>
#include <gtest/gtest.h>
#include <vector>

// [Writing Functions Taking Eigen Types as Parameters](https://eigen.tuxfamily.org/dox/TopicFunctionTakingEigenTypes.html)

// Matrix with fixed 3x3 capacity, but dynamic size.
using FixedMatrix =
        Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, 0, 3, 3>;

// Matrix with dynamic capacity and size.
using DynamicMatrix = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>;

// A reference to any matrix with dynamic size (not just DynamicMatrix) or a block.
using MatrixRef = Eigen::Ref<DynamicMatrix>;

// A matrix expression.
template <typename Derived>
using MatrixExp = Eigen::MatrixBase<Derived>;

// Resize a dynamic matrix.
// Resize a fixed matrix beyond its capacity?

// Templated function with in/out expression parameters.
// Templated function with in expression parameters that returns a scalar.
// Templated function with in expression parameters that returns a matrix.
// Temmplated function with in expression parameters that returns an expression.

// Non-templated function with in and in/out parameters. (No copies)
// Non-templated function with in and in/out block parameters. (No copies)

// Intermediate variable without evaluation? Use auto&& ?

class TestEigen : public testing::Test {
public:
    using FixedVector = Eigen::Vector3d;
    using MultiVector = Eigen::Matrix<double, 3, Eigen::Dynamic>;
    using Vector = Eigen::VectorXd;
    using Matrix = Eigen::MatrixXd;

    // Initialize all data structures (with the same random data)
    // during setup in order to exclude this work from timing results.
    void SetUp() override {
        a4.setRandom();
        b4.setRandom();
        a3 = a4;
        b3 = b4;
        for (auto i = 0; i < aSize; i++) {
            a2[i] = a4.col(i);
            a1[i] = a4.col(i);
        }
        for (auto i = 0; i < bSize; i++) {
            b2[i] = b4.col(i);
            b1[i] = b4.col(i);
        }
        for (auto row = 0; row < expected.rows(); row++) {
            for (auto col = 0; col < expected.cols(); col++) {
                expected(row, col) = (a4.col(row) - b4.col(col)).squaredNorm();
            }
        }
    }

    // Accept arguments by non-const lvalue reference
    // in order to prevent creation of temporary objects.
    void expectEq([[maybe_unused]] Matrix& actual, [[maybe_unused]] Matrix& expected) {
#ifndef NDEBUG
        EXPECT_TRUE(actual.isApprox(expected));
#endif // NDEBUG
    }

#ifdef NDEBUG
    static constexpr int aSize{10000};
    static constexpr int bSize{15000};
#else
    static constexpr int aSize{100};
    static constexpr int bSize{150};
#endif

    // Collection of fixed-capacity vectors.
    std::vector<FixedVector> a1{aSize};
    std::vector<FixedVector> b1{bSize};

    // Collection of dynamic-capacity vectors.
    std::vector<Vector> a2{aSize, Vector{3}};
    std::vector<Vector> b2{bSize, Vector{3}};

    // Matrix packed with fixed-capacity vectors.
    MultiVector a3{3, aSize};
    MultiVector b3{3, bSize};

    // Matrix packed with dynamic-capacity vectors.
    Matrix a4{3, aSize};
    Matrix b4{3, bSize};

    Matrix expected{aSize, bSize};
};

TEST_F(TestEigen, squared_norm_collection_fixed) {
    // Pack a collection of vectors into a matrix
    // to achieve "struct of arrays" efficiency
    auto n = Eigen::MatrixXd{a1.size(), b1.size()};
    for (auto row = 0; row < n.rows(); row++) {
        for (auto col = 0; col < n.cols(); col++) {
            n(row, col) = (a1[row] - b1[col]).squaredNorm();
        }
    }
    expectEq(n, expected);
}

TEST_F(TestEigen, squared_norm_collection_dynamic) {
    auto n = Eigen::MatrixXd{a2.size(), b2.size()};
    for (auto row = 0; row < n.rows(); row++) {
        for (auto col = 0; col < n.cols(); col++) {
            n(row, col) = (a2[row] - b2[col]).squaredNorm();
        }
    }
    expectEq(n, expected);
}

TEST_F(TestEigen, squared_norm_packed_fixed) {
    // Pack a collection of vectors into a matrix
    // to achieve "struct of arrays" efficiency
    auto n = Eigen::MatrixXd{a3.cols(), b3.cols()};
    for (auto row = 0; row < n.rows(); row++) {
        for (auto col = 0; col < n.cols(); col++) {
            n(row, col) = (a3.col(row) - b3.col(col)).squaredNorm();
        }
    }
    expectEq(n, expected);
}

TEST_F(TestEigen, squared_norm_packed_dynamic) {
    auto n = Eigen::MatrixXd{a4.cols(), b4.cols()};
    for (auto row = 0; row < n.rows(); row++) {
        for (auto col = 0; col < n.cols(); col++) {
            n(row, col) = (a4.col(row) - b4.col(col)).squaredNorm();
        }
    }
    expectEq(n, expected);
}

TEST_F(TestEigen, colwise_squared_norm_packed_dynamic) {
    auto n = Eigen::MatrixXd{a4.cols(), b4.cols()};
    for (auto row = 0; row < n.rows(); row++) {
        n.row(row) = (b4.colwise() - a4.col(row)).colwise().squaredNorm();
    }
    expectEq(n, expected);
}

TEST_F(TestEigen, abs_norm_packed_dynamic) {
    auto n = Eigen::MatrixXd{a4.cols(), b4.cols()};
    for (auto row = 0; row < n.rows(); row++) {
        for (auto col = 0; col < n.cols(); col++) {
            n(row, col) = (a4.col(row) - b4.col(col)).cwiseAbs().sum();
        }
    }
}

TEST_F(TestEigen, 1_norm_packed_dynamic) {
    auto n = Eigen::MatrixXd{a4.cols(), b4.cols()};
    for (auto row = 0; row < n.rows(); row++) {
        for (auto col = 0; col < n.cols(); col++) {
            n(row, col) = (a4.col(row) - b4.col(col)).lpNorm<1>();
        }
    }
}

TEST_F(TestEigen, 2_norm_packed_dynamic) {
    auto n = Eigen::MatrixXd{a4.cols(), b4.cols()};
    for (auto row = 0; row < n.rows(); row++) {
        for (auto col = 0; col < n.cols(); col++) {
            n(row, col) = (a4.col(row) - b4.col(col)).norm();
        }
    }
}
