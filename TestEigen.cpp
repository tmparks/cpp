#include <Eigen/Eigen>
#include <boost/timer/timer.hpp>
#include <gtest/gtest.h>
#include <vector>

// [Writing Functions Taking Eigen Types as Parameters](https://eigen.tuxfamily.org/dox/TopicFunctionTakingEigenTypes.html)

// Resize a dynamic matrix.
// Resize a fixed matrix beyond its capacity?

// Templated function with in/out expression parameters.
// Templated function with in expression parameters that returns a scalar.
// Templated function with in expression parameters that returns a matrix.
// Temmplated function with in expression parameters that returns an expression.

// Non-templated function with in and in/out parameters. (No copies)
// Non-templated function with in and in/out block parameters. (No copies)

// Intermediate variable without evaluation? Use auto&& ?

// Logical indexing

// Data structure containing an Eigen fixed-capacity vector/matrix.
// Look for alignment issues.

class TestEigen : public testing::Test {
public:
    // Default storage options.
    static constexpr auto Default = Eigen::StorageOptions{};

    // Fixed-size column vector.
    using FixedVector = Eigen::Vector3d;

    // Fixed-capacity, dynamically sized column vector.
    // Capacity is 4 to achieve 16-byte alignment.
    using CappedVector = Eigen::Matrix<double, Eigen::Dynamic, 1, Default, 4, 1>;

    // Dynamically sized column vector.
    using Vector = Eigen::VectorXd;

    // Reference to any column vector or block.
    using VectorRef = Eigen::Ref<Vector>;
    using VectorConstRef = const Eigen::Ref<const Vector>;

    // Fixed-size column vectors packed into a dynamically-sized matrix.
    using MultiFixedVector = Eigen::Matrix<double, 3, Eigen::Dynamic>;

    // Fixed-capacity column vectors packed into a dynamically-sized matrix.
    // Capacity is 4 to achieve 16-byte alignment.
    using MultiCappedVector =
            Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Default, 4, Eigen::Dynamic>;

    // Dynamically sized matrix.
    using Matrix = Eigen::MatrixXd;

    // Reference to any matrix or block.
    using MatrixRef = Eigen::Ref<Matrix>;
    using MatrixConstRef = const Eigen::Ref<const Matrix>;

    // A matrix (or vector) expression.
    template <typename Derived>
    using MatrixExp = Eigen::MatrixBase<Derived>;

    static void SetUpTestCase() {
        std::cout << "size of a + b + n = "
                  << (sizeof(double) * ((aSize * bSize) + 3 * (aSize + bSize)))
                  << std::endl;
    }

    // Initialize all data structures (with the same random data)
    // during setup in order to exclude this work from timing results.
    void SetUp() override {
        aDynamic.setRandom(3, aSize);
        bDynamic.setRandom(3, bSize);
        aCapped = aDynamic;
        bCapped = bDynamic;
        aFixed = aDynamic;
        bFixed = bDynamic;

        aPointerCollection.resize(aSize);
        aDynamicCollection.resize(aSize);
        aCappedCollection.resize(aSize);
        aFixedCollection.resize(aSize);
        for (auto i = 0; i < aSize; i++) {
            aPointerCollection[i] = std::make_unique<Vector>(aDynamic.col(i));
            aDynamicCollection[i] = aDynamic.col(i);
            aCappedCollection[i] = aDynamic.col(i);
            aFixedCollection[i] = aDynamic.col(i);
        }

        bPointerCollection.resize(bSize);
        bDynamicCollection.resize(bSize);
        bCappedCollection.resize(bSize);
        bFixedCollection.resize(bSize);
        for (auto i = 0; i < bSize; i++) {
            bPointerCollection[i] = std::make_unique<Vector>(bDynamic.col(i));
            bDynamicCollection[i] = bDynamic.col(i);
            bCappedCollection[i] = bDynamic.col(i);
            bFixedCollection[i] = bDynamic.col(i);
        }

        actual.setZero(aSize, bSize);
        expected.setZero(aSize, bSize);
        for (auto row = 0; row < expected.rows(); row++) {
            for (auto col = 0; col < expected.cols(); col++) {
                expected(row, col) =
                        (aDynamic.col(row) - bDynamic.col(col)).squaredNorm();
            }
        }
        timer.start();
    }

    void TearDown() override {
        timer.stop();
        expectEq(actual, expected);
        std::cout << timer.format(3) << std::endl;
    }

    // Accept arguments by non-const lvalue reference
    // in order to prevent creation of temporary objects.
    void expectEq(Matrix& actual, Matrix& expected) {
        if (actual(0, 0) != 0) {
            EXPECT_DOUBLE_EQ(actual(0, 0), expected(0, 0));
        }
        EXPECT_TRUE(actual.isApprox(expected) or actual.isZero());
    }

    // Benchmark for collection of vectors.
    template <typename T>
    void squaredNorm(const std::vector<T>& a, const std::vector<T>& b) {
        for (int i = 0; i < repeat; i++) {
            for (auto row = 0; row < actual.rows(); row++) {
                for (auto col = 0; col < actual.cols(); col++) {
                    actual(row, col) = (a[row] - b[col]).squaredNorm();
                }
            }
        }
    }

    // Benchmark for matrix types.
    template <typename T>
    void squaredNorm(const Eigen::MatrixBase<T>& a, const Eigen::MatrixBase<T>& b) {
        for (int i = 0; i < repeat; i++) {
            for (auto row = 0; row < actual.rows(); row++) {
                for (auto col = 0; col < actual.cols(); col++) {
                    actual(row, col) = (a.col(row) - b.col(col)).squaredNorm();
                }
            }
        }
    }

    // Column-wise benchmark.
    template <typename T>
    void squaredNormColwise(
            const Eigen::MatrixBase<T>& a, const Eigen::MatrixBase<T>& b) {
        for (int i = 0; i < repeat; i++) {
            for (auto col = 0; col < actual.cols(); col++) {
                actual.col(col) =
                        (a.colwise() - b.col(col)).colwise().squaredNorm();
            }
        }
    }

    double squaredNormRef(VectorConstRef a, VectorConstRef b) {
        return (a - b).squaredNorm();
    }

    auto squaredNormAuto(const auto& a, const auto& b) {
        return (a - b).squaredNorm();
    }

#ifdef NDEBUG
    static constexpr int aSize{40};
    static constexpr int bSize{150};
    static constexpr int repeat = 100'000;
#else
    static constexpr int aSize{40};
    static constexpr int bSize{150};
    static constexpr int repeat = 3;
#endif

    // Collection of fixed-size vectors.
    std::vector<FixedVector> aFixedCollection;
    std::vector<FixedVector> bFixedCollection;

    // Collection of fixed-capacity vectors.
    std::vector<CappedVector> aCappedCollection;
    std::vector<CappedVector> bCappedCollection;

    // Collection of dynamic-capacity vectors.
    std::vector<Vector> aDynamicCollection;
    std::vector<Vector> bDynamicCollection;

    // Collection of pointers to dynamic-capacity vectors.
    std::vector<std::unique_ptr<Vector>> aPointerCollection;
    std::vector<std::unique_ptr<Vector>> bPointerCollection;

    // Matrix packed with fixed-size vectors.
    MultiFixedVector aFixed;
    MultiFixedVector bFixed;

    // Matrix packed with fixed-capacity vectors.
    MultiCappedVector aCapped;
    MultiCappedVector bCapped;

    // Matrix packed with dynamic-capacity vectors.
    Matrix aDynamic;
    Matrix bDynamic;

    Matrix actual;
    Matrix expected;

    boost::timer::cpu_timer timer;
};

TEST_F(TestEigen, sizeof) {
    auto vectorDataSize = 3 * sizeof(double);
    auto matrixDataSize = aSize * vectorDataSize;

    // Pointer, size.
    auto dynamicVectorSize = sizeof(double*) + sizeof(size_t);

    // Pointer, rows, columns.
    auto dynamicMatrixSize = sizeof(double*) + 2 * sizeof(size_t);

    EXPECT_EQ(sizeof(FixedVector), vectorDataSize) << "expect no overhead";
    EXPECT_EQ(sizeof(Vector), dynamicVectorSize);
    EXPECT_EQ(sizeof(MultiFixedVector), dynamicMatrixSize - sizeof(size_t))
            << "expect no row count";
    EXPECT_EQ(sizeof(Matrix), dynamicMatrixSize);

    EXPECT_GE(sizeof(VectorRef), dynamicVectorSize);
    // EXPECT_EQ(sizeof(VectorConstRef), sizeof(VectorRef));

    EXPECT_GE(sizeof(MatrixRef), dynamicMatrixSize);
    // EXPECT_EQ(sizeof(MatrixConstRef), sizeof(MatrixRef));

    EXPECT_LT(sizeof(aFixedCollection), matrixDataSize);
    EXPECT_LT(sizeof(aDynamicCollection), matrixDataSize);
    EXPECT_LT(sizeof(aPointerCollection), matrixDataSize);
    EXPECT_LT(sizeof(aFixed), matrixDataSize);
    EXPECT_LT(sizeof(aDynamic), matrixDataSize);
}

TEST_F(TestEigen, collection_fixed) {
    squaredNorm(aFixedCollection, bFixedCollection);
}

TEST_F(TestEigen, collection_capped) {
    squaredNorm(aCappedCollection, bCappedCollection);
}

TEST_F(TestEigen, collection_dynamic) {
    squaredNorm(aDynamicCollection, bDynamicCollection);
}

TEST_F(TestEigen, collection_pointer) {
    auto& a = aPointerCollection;
    auto& b = bPointerCollection;
    for (int i = 0; i < repeat; i++) {
        for (auto row = 0; row < actual.rows(); row++) {
            for (auto col = 0; col < actual.cols(); col++) {
                actual(row, col) = (*a[row] - *b[col]).squaredNorm();
            }
        }
    }
}

TEST_F(TestEigen, packed_fixed) { squaredNorm(aFixed, bFixed); }

TEST_F(TestEigen, packed_capped) { squaredNorm(aCapped, bCapped); }

TEST_F(TestEigen, packed_dynamic) { squaredNorm(aDynamic, bDynamic); }

TEST_F(TestEigen, colwise_fixed) { squaredNormColwise(aFixed, bFixed); }

TEST_F(TestEigen, colwise_capped) { squaredNormColwise(aCapped, bCapped); }

TEST_F(TestEigen, colwise_dynamic) { squaredNormColwise(aDynamic, bDynamic); }

TEST_F(TestEigen, abs_norm_packed_dynamic) {
    auto& a = aDynamic;
    auto& b = bDynamic;
    for (int i = 0; i < repeat; i++) {
        for (auto row = 0; row < actual.rows(); row++) {
            for (auto col = 0; col < actual.cols(); col++) {
                actual(row, col) = (a.col(row) - b.col(col)).cwiseAbs().sum();
            }
        }
    }
    actual.setZero(); // do not check results
}

TEST_F(TestEigen, 1_norm_packed_dynamic) {
    auto& a = aDynamic;
    auto& b = bDynamic;
    for (int i = 0; i < repeat; i++) {
        for (auto row = 0; row < actual.rows(); row++) {
            for (auto col = 0; col < actual.cols(); col++) {
                actual(row, col) = (a.col(row) - b.col(col)).lpNorm<1>();
            }
        }
    }
    actual.setZero(); // do not check results
}

TEST_F(TestEigen, 2_norm_packed_dynamic) {
    auto& a = aDynamic;
    auto& b = bDynamic;
    for (int i = 0; i < repeat; i++) {
        for (auto row = 0; row < actual.rows(); row++) {
            for (auto col = 0; col < actual.cols(); col++) {
                actual(row, col) = (a.col(row) - b.col(col)).norm();
            }
        }
    }
    actual.setZero(); // do not check results
}
