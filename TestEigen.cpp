// NOLINTNEXTLINE(*-macro-usage)
#define EIGEN_RUNTIME_NO_MALLOC 1

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
    using CappedVector = Eigen::Matrix<double, Eigen::Dynamic, 1, Default, 4, 1>;

    // Dynamically sized column vector.
    using Vector = Eigen::VectorXd;

    // Reference to any column vector or block.
    using VectorRef = Eigen::Ref<Vector>;
    using VectorConstRef = const Eigen::Ref<const Vector>;

    // Fixed-size column vectors packed into a dynamically-sized matrix.
    using MultiFixedVector = Eigen::Matrix<double, 3, Eigen::Dynamic>;

    // Fixed-capacity column vectors packed into a dynamically-sized matrix.
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
        Eigen::internal::set_is_malloc_allowed(false);
        timer.start();
    }

    void TearDown() override {
        timer.stop();
        Eigen::internal::set_is_malloc_allowed(true);
        expectEq(actual, expected);
        std::cout << timer.format(3);
    }

    // Accept arguments by non-const lvalue reference
    // in order to prevent creation of temporary objects.
    // Compilation fails if types are mismatched.
    void expectEq(Matrix& actual, Matrix& expected) {
        if (actual(0, 0) != 0) {
            EXPECT_DOUBLE_EQ(actual(0, 0), expected(0, 0));
        }
        EXPECT_TRUE(actual.isApprox(expected) or actual.isZero());
    }

    // Benchmark for collection of vectors.
    template <typename A, typename B>
    void squaredDistance(const std::vector<A>& a, const std::vector<B>& b) {
        for (auto i = 0; i < repeat; i++) {
            for (auto row = 0; row < actual.rows(); row++) {
                for (auto col = 0; col < actual.cols(); col++) {
                    actual(row, col) = (a[row] - b[col]).squaredNorm();
                }
            }
        }
    }

    // Benchmark for matrix types.
    template <typename A, typename B>
    void squaredDistance(
            const Eigen::MatrixBase<A>& a, const Eigen::MatrixBase<B>& b) {
        for (auto i = 0; i < repeat; i++) {
            for (auto row = 0; row < actual.rows(); row++) {
                for (auto col = 0; col < actual.cols(); col++) {
                    actual(row, col) = (a.col(row) - b.col(col)).squaredNorm();
                }
            }
        }
    }

    // Column-wise benchmark.
    template <typename A, typename B>
    void squaredDistanceColwise(
            const Eigen::MatrixBase<A>& a, const Eigen::MatrixBase<B>& b) {
        for (auto i = 0; i < repeat; i++) {
            for (auto col = 0; col < actual.cols(); col++) {
                actual.col(col) =
                        (a.colwise() - b.col(col)).colwise().squaredNorm();
            }
        }
    }

    // Replicate benchmark.
    template <typename A, typename B>
    void squaredDistanceReplicate(
            const Eigen::MatrixBase<A>& a, const Eigen::MatrixBase<B>& b) {
        for (auto i = 0; i < repeat; i++) {
            for (auto col = 0; col < actual.cols(); col++) {
                actual.col(col) = (a - b.col(col).rowwise().replicate(a.cols()))
                                          .colwise()
                                          .squaredNorm();
            }
        }
    }

    // Parallel benchmark.
    template <typename A, typename B>
    void squaredDistanceParallel(
            const Eigen::MatrixBase<A>& a, const Eigen::MatrixBase<B>& b) {
        for (auto i = 0; i < repeat; i++) {
#pragma omp parallel for num_threads(2)
            for (auto col = 0; col < actual.cols(); col++) {
                actual.col(col) =
                        (a.colwise() - b.col(col)).colwise().squaredNorm();
            }
        }
    }

    // No temporary objects are created when arguments are any matrix (or vector)
    // expression, but not a special matrix.
    template <typename A, typename B>
    double squaredDistanceTemplate(
            const Eigen::MatrixBase<A>& a, const Eigen::MatrixBase<B>& b) {
        return (a - b).squaredNorm();
    }

    // No temporary objects are created when arguments are any vector or block.
    double squaredDistanceRef(VectorConstRef a, VectorConstRef b) {
        return (a - b).squaredNorm();
    }

    // No temporary objects are created when arguments are FixedVector.
    double squaredDistanceFixed(const FixedVector& a, const FixedVector& b) {
        return (a - b).squaredNorm();
    }

    // No temporary objects are created when arguments are CappedVector.
    double squaredDistanceCapped(const CappedVector& a, const CappedVector& b) {
        return (a - b).squaredNorm();
    }

    // No temporary objects are created when arguments are Vector.
    double squaredDistanceDynamic(const Vector& a, const Vector& b) {
        return (a - b).squaredNorm();
    }

    // Verify that temporary objects are not created by comparing data pointers.
    template <typename T>
    bool sameDataTemplate(const Eigen::PlainObjectBase<T>& x, const double* data) {
        return x.data() == data;
    }

    // Verify that temporary objects are not created by comparing data pointers.
    bool sameDataRef(MatrixRef x, const double* data) {
        return x.data() == data;
    }

    // Verify that temporary objects are not created by comparing data pointers.
    bool sameDataConstRef(MatrixConstRef x, const double* data) {
        return x.data() == data;
    }

#if __cplusplus >= 202002L // since C++20

    // No temporary objects are created.
    auto squaredDistanceAuto(const auto& a, const auto& b) {
        return (a - b).squaredNorm();
    }

    // Verify that temporary objects are not created by comparing data pointers.
    bool sameDataAuto(const auto& x, const double* data) {
        return x.data() == data;
    }

#endif // C++20

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

    EXPECT_EQ(sizeof(VectorRef), dynamicVectorSize + sizeof(size_t))
            << "expect added stride";
    EXPECT_EQ(sizeof(VectorConstRef), sizeof(VectorRef) + dynamicVectorSize)
            << "expect added object";

    EXPECT_GE(sizeof(MatrixRef), dynamicMatrixSize);
    EXPECT_EQ(sizeof(MatrixConstRef), sizeof(MatrixRef) + dynamicMatrixSize)
            << "expect added object";

    EXPECT_LT(sizeof(aFixedCollection), matrixDataSize);
    EXPECT_LT(sizeof(aDynamicCollection), matrixDataSize);
    EXPECT_LT(sizeof(aPointerCollection), matrixDataSize);
    EXPECT_LT(sizeof(aFixed), matrixDataSize);
    EXPECT_LT(sizeof(aDynamic), matrixDataSize);
}

// [Structures Having Eigen Members](https://eigen.tuxfamily.org/dox/group__TopicStructHavingEigenMembers.html)
TEST_F(TestEigen, alignment) {
    Eigen::internal::set_is_malloc_allowed(true);
    auto desiredAlignment = 2 * sizeof(double);

    struct aligned {
        double d1{};
        Eigen::Matrix4d m{};
        double d2{};
    } a;
    auto pa = std::make_unique<aligned>();
    // NOLINTNEXTLINE(*-reinterpret-cast)
    auto pam = reinterpret_cast<std::ptrdiff_t>(std::addressof(pa->m));

    EXPECT_GT(sizeof(a), 18 * sizeof(double)) << "expect gaps";
    EXPECT_EQ(offsetof(aligned, m) % desiredAlignment, 0);
    EXPECT_EQ(pam % desiredAlignment, 0);

    struct misaligned {
        double d1{};
        Eigen::Matrix<double, 4, 4, Eigen::DontAlign> m{};
        double d2{};
    } b;
    auto pb = std::make_unique<misaligned>();
    // NOLINTNEXTLINE(*-reinterpret-cast)
    auto pbm = reinterpret_cast<std::ptrdiff_t>(std::addressof(pb->m));

    EXPECT_EQ(sizeof(b), 18 * sizeof(double)) << "expect no gaps";
    EXPECT_NE(offsetof(misaligned, m) % desiredAlignment, 0);
    EXPECT_NE(pbm % desiredAlignment, 0);
}

TEST_F(TestEigen, collectionFixed) {
    squaredDistance(aFixedCollection, bFixedCollection);
}

TEST_F(TestEigen, collectionCapped) {
    squaredDistance(aCappedCollection, bCappedCollection);
}

TEST_F(TestEigen, collectionDynamic) {
    squaredDistance(aDynamicCollection, bDynamicCollection);
}

TEST_F(TestEigen, collectionMixed) {
    squaredDistance(aFixedCollection, bDynamicCollection);
}

TEST_F(TestEigen, collectionPointer) {
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

TEST_F(TestEigen, packedFixed) { squaredDistance(aFixed, bFixed); }

TEST_F(TestEigen, packedCapped) { squaredDistance(aCapped, bCapped); }

TEST_F(TestEigen, packedDynamic) { squaredDistance(aDynamic, bDynamic); }

TEST_F(TestEigen, packedMixed) { squaredDistance(aFixed, bDynamic); }

TEST_F(TestEigen, colwiseFixed) { squaredDistanceColwise(aFixed, bFixed); }

TEST_F(TestEigen, colwiseCapped) { squaredDistanceColwise(aCapped, bCapped); }

TEST_F(TestEigen, colwiseDynamic) {
    squaredDistanceColwise(aDynamic, bDynamic);
}

TEST_F(TestEigen, colwiseMixed) { squaredDistanceColwise(aFixed, bDynamic); }

TEST_F(TestEigen, replicateFixed) { squaredDistanceReplicate(aFixed, bFixed); }

TEST_F(TestEigen, replicateCapped) {
    squaredDistanceReplicate(aCapped, bCapped);
}

TEST_F(TestEigen, replicateDynamic) {
    squaredDistanceReplicate(aDynamic, bDynamic);
}

TEST_F(TestEigen, replicateMixed) {
    squaredDistanceReplicate(aFixed, bDynamic);
}

TEST_F(TestEigen, parallelFixed) { squaredDistanceParallel(aFixed, bFixed); }

TEST_F(TestEigen, parallelCapped) { squaredDistanceParallel(aCapped, bCapped); }

TEST_F(TestEigen, parallelDynamic) {
    squaredDistanceParallel(aDynamic, bDynamic);
}

TEST_F(TestEigen, parallelMixed) { squaredDistanceParallel(aFixed, bDynamic); }

TEST_F(TestEigen, templateMixed) {
    auto& a = aFixed;
    auto& b = bDynamic;
    for (int i = 0; i < repeat; i++) {
        for (auto row = 0; row < actual.rows(); row++) {
            for (auto col = 0; col < actual.cols(); col++) {
                actual(row, col) =
                        squaredDistanceTemplate(a.col(row), b.col(col));
            }
        }
    }
}

TEST_F(TestEigen, refMixed) {
    auto& a = aFixed;
    auto& b = bDynamic;
    for (int i = 0; i < repeat; i++) {
        for (auto row = 0; row < actual.rows(); row++) {
            for (auto col = 0; col < actual.cols(); col++) {
                actual(row, col) = squaredDistanceRef(a.col(row), b.col(col));
            }
        }
    }
}

TEST_F(TestEigen, fixedMixed) {
    auto& a = aFixed;
    auto& b = bDynamic;
    for (int i = 0; i < repeat; i++) {
        for (auto row = 0; row < actual.rows(); row++) {
            for (auto col = 0; col < actual.cols(); col++) {
                actual(row, col) = squaredDistanceFixed(a.col(row), b.col(col));
            }
        }
    }
}

TEST_F(TestEigen, cappedMixed) {
    auto& a = aFixed;
    auto& b = bDynamic;
    for (int i = 0; i < repeat; i++) {
        for (auto row = 0; row < actual.rows(); row++) {
            for (auto col = 0; col < actual.cols(); col++) {
                actual(row, col) = squaredDistanceCapped(a.col(row), b.col(col));
            }
        }
    }
}

TEST_F(TestEigen, dynamicMixed) {
    // Arguments to squaredDistanceDynamic() will be converted to
    // temporary dynamic-capacity vectors, causing memory allocation
    Eigen::internal::set_is_malloc_allowed(true);

    auto& a = aFixed;
    auto& b = bDynamic;
    for (int i = 0; i < repeat / 10; i++) { // NOLINT(*-avoid-magic-numbers)
        for (auto row = 0; row < actual.rows(); row++) {
            for (auto col = 0; col < actual.cols(); col++) {
                actual(row, col) = squaredDistanceDynamic(a.col(row), b.col(col));
            }
        }
    }
}

TEST_F(TestEigen, absDynamic) {
    auto& a = aDynamic;
    auto& b = bDynamic;
    for (int i = 0; i < repeat; i++) {
        for (auto row = 0; row < actual.rows(); row++) {
            for (auto col = 0; col < actual.cols(); col++) {
                actual(row, col) = (a.col(row) - b.col(col)).cwiseAbs().sum();
            }
        }
    }
    timer.stop();
    actual.setZero(); // do not check results
}

TEST_F(TestEigen, 1Dynamic) {
    auto& a = aDynamic;
    auto& b = bDynamic;
    for (int i = 0; i < repeat; i++) {
        for (auto row = 0; row < actual.rows(); row++) {
            for (auto col = 0; col < actual.cols(); col++) {
                actual(row, col) = (a.col(row) - b.col(col)).lpNorm<1>();
            }
        }
    }
    timer.stop();
    actual.setZero(); // do not check results
}

TEST_F(TestEigen, 2Dynamic) {
    auto& a = aDynamic;
    auto& b = bDynamic;
    for (int i = 0; i < repeat; i++) {
        for (auto row = 0; row < actual.rows(); row++) {
            for (auto col = 0; col < actual.cols(); col++) {
                actual(row, col) = (a.col(row) - b.col(col)).norm();
            }
        }
    }
    timer.stop();
    actual = actual.array().square(); // adjust result
}

TEST_F(TestEigen, sameDataFixed) {
    EXPECT_TRUE(sameDataTemplate(aFixed, aFixed.data()));
    EXPECT_TRUE(sameDataRef(aFixed, aFixed.data()));
    EXPECT_TRUE(sameDataConstRef(aFixed, aFixed.data()));
}

TEST_F(TestEigen, sameDataCapped) {
    EXPECT_TRUE(sameDataTemplate(aCapped, aCapped.data()));
    EXPECT_TRUE(sameDataRef(aCapped, aCapped.data()));
    EXPECT_TRUE(sameDataConstRef(aCapped, aCapped.data()));
}

TEST_F(TestEigen, sameDataDynamic) {
    EXPECT_TRUE(sameDataTemplate(aDynamic, aDynamic.data()));
    EXPECT_TRUE(sameDataRef(aDynamic, aDynamic.data()));
    EXPECT_TRUE(sameDataConstRef(aDynamic, aDynamic.data()));
}

TEST_F(TestEigen, sameDataBlock) {
    EXPECT_TRUE(sameDataRef(aDynamic.leftCols(1), aDynamic.data()));
    EXPECT_TRUE(sameDataRef(aDynamic.topRows(1), aDynamic.data()));
    EXPECT_FALSE(sameDataRef(aDynamic.rightCols(1), aDynamic.data()));
    EXPECT_FALSE(sameDataRef(aDynamic.bottomRows(1), aDynamic.data()));

    EXPECT_TRUE(sameDataConstRef(aDynamic.leftCols(1), aDynamic.data()));
    EXPECT_TRUE(sameDataConstRef(aDynamic.topRows(1), aDynamic.data()));
    EXPECT_FALSE(sameDataConstRef(aDynamic.rightCols(1), aDynamic.data()));
    EXPECT_FALSE(sameDataConstRef(aDynamic.bottomRows(1), aDynamic.data()));
}

#if __cplusplus >= 202002L // since C++20

TEST_F(TestEigen, sameDataAuto) {
    EXPECT_TRUE(sameDataAuto(aFixed, aFixed.data()));
    EXPECT_TRUE(sameDataAuto(aCapped, aCapped.data()));
    EXPECT_TRUE(sameDataAuto(aDynamic, aDynamic.data()));
}

TEST_F(TestEigen, autoMixed) {
    auto& a = aFixed;
    auto& b = bDynamic;
    for (int i = 0; i < repeat; i++) {
        for (auto row = 0; row < actual.rows(); row++) {
            for (auto col = 0; col < actual.cols(); col++) {
                actual(row, col) = squaredDistanceAuto(a.col(row), b.col(col));
            }
        }
    }
}

#endif // C++20
