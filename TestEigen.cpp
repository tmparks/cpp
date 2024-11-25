// NOLINTNEXTLINE(*-macro-usage)
#define EIGEN_RUNTIME_NO_MALLOC 1

#include <Eigen/Eigen>
#include <boost/timer/timer.hpp>
#include <gtest/gtest.h>
#include <vector>

using namespace Eigen;

// [Writing Functions Taking Eigen Types as Parameters](https://eigen.tuxfamily.org/dox/TopicFunctionTakingEigenTypes.html)

// Templated function with in expression parameters that returns a matrix.
// Templated function with in expression parameters that returns an expression.

// Intermediate variable without evaluation? Use auto&& ?

class TestEigen : public testing::Test {
public:
    // Fixed-capacity, dynamically sized column vector.
    using VectorX4d = Matrix<double, Dynamic, 1, StorageOptions{}, 4, 1>;

    // Fixed-capacity column vectors packed into a dynamically-sized matrix.
    using MatrixXX4Xd =
            Matrix<double, Dynamic, Dynamic, StorageOptions{}, 4, Dynamic>;

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
            aPointerCollection[i] = std::make_unique<VectorXd>(aDynamic.col(i));
            aDynamicCollection[i] = aDynamic.col(i);
            aCappedCollection[i] = aDynamic.col(i);
            aFixedCollection[i] = aDynamic.col(i);
        }

        bPointerCollection.resize(bSize);
        bDynamicCollection.resize(bSize);
        bCappedCollection.resize(bSize);
        bFixedCollection.resize(bSize);
        for (auto i = 0; i < bSize; i++) {
            bPointerCollection[i] = std::make_unique<VectorXd>(bDynamic.col(i));
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
    void expectEq(MatrixXd& actual, MatrixXd& expected) {
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
    void squaredDistance(const MatrixBase<A>& a, const MatrixBase<B>& b) {
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
    void squaredDistanceColwise(const MatrixBase<A>& a, const MatrixBase<B>& b) {
        for (auto i = 0; i < repeat; i++) {
            for (auto col = 0; col < actual.cols(); col++) {
                actual.col(col) =
                        (a.colwise() - b.col(col)).colwise().squaredNorm();
            }
        }
    }

    // Replicate benchmark.
    template <typename A, typename B>
    void squaredDistanceReplicate(const MatrixBase<A>& a, const MatrixBase<B>& b) {
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
    void squaredDistanceParallel(const MatrixBase<A>& a, const MatrixBase<B>& b) {
        for (auto i = 0; i < repeat; i++) {
#pragma omp parallel for num_threads(2)
            for (auto col = 0; col < actual.cols(); col++) {
                actual.col(col) =
                        (a.colwise() - b.col(col)).colwise().squaredNorm();
            }
        }
    }

    // No temporary objects are created when arguments are any matrix (or vector)
    // expression (but not a special matrix such as Eigen::DiagonalMatrix)
    template <typename A, typename B>
    double squaredDistanceTemplate(const MatrixBase<A>& a, const MatrixBase<B>& b) {
        return (a - b).squaredNorm();
    }

    // No temporary objects are created when arguments are any vector or subvector.
    double squaredDistanceRef(Ref<VectorXd> a, Ref<VectorXd> b) {
        return (a - b).squaredNorm();
    }

    // No temporary objects are created when arguments are any vector or subvector.
    double squaredDistanceConstRef(
            const Ref<const VectorXd>& a, const Ref<const VectorXd>& b) {
        return (a - b).squaredNorm();
    }

    // No temporary objects are created when arguments are any vector or subvector.
    double squaredDistanceFixedRef(Ref<Vector3d> a, Ref<Vector3d> b) {
        return (a - b).squaredNorm();
    }

    // No temporary objects are created when arguments are any vector or subvector.
    double squaredDistanceFixedConstRef(
            const Ref<const Vector3d>& a, const Ref<const Vector3d>& b) {
        return (a - b).squaredNorm();
    }

    // No temporary objects are created when arguments are Vector3d.
    double squaredDistanceFixed(const Vector3d& a, const Vector3d& b) {
        return (a - b).squaredNorm();
    }

    // No temporary objects are created when arguments are VectorX4d.
    double squaredDistanceCapped(const VectorX4d& a, const VectorX4d& b) {
        return (a - b).squaredNorm();
    }

    // No temporary objects are created when arguments are VectorXd.
    double squaredDistanceDynamic(const VectorXd& a, const VectorXd& b) {
        return (a - b).squaredNorm();
    }

    // Verify that temporary objects are not created by comparing data pointers.
    template <typename T>
    bool sameDataTemplate(const MatrixBase<T>& x, const double* data) {
        return x.derived().data() == data;
    }

    // Verify that temporary objects are not created by comparing data pointers.
    bool sameDataRef(Ref<MatrixXd> x, const double* data) {
        return x.data() == data;
    }

    // Verify that temporary objects are not created by comparing data pointers.
    bool sameDataConstRef(const Ref<const MatrixXd>& x, const double* data) {
        return x.data() == data;
    }

    // Verify that temporary objects are not created by comparing data pointers.
    bool sameData(const MatrixXd& x, const double* data) {
        return x.data() == data;
    }

    // Extract a read-write block from any matrix expression
    // without creating temporary objects.
    template <typename Derived>
    Block<Derived> columnTemplate(MatrixBase<Derived>& x, int col) {
        return x.block(0, col, x.rows(), 1);
    }

    // Extract a read-only block from any matrix expression
    // without creating temporary objects.
    template <typename Derived>
    const Block<const Derived> columnTemplate(const MatrixBase<Derived>& x, int col) {
        return x.block(0, col, x.rows(), 1);
    }

    // Extract a read-write block from any matrix type
    // without creating temporary objects.
    Block<Ref<MatrixXd>> columnRef(Ref<MatrixXd> x, int col) {
        return x.block(0, col, x.rows(), 1);
    }

    // Extract a read-only block from any matrix type
    // without creating temporary objects.
    const Block<const Ref<const MatrixXd>> columnConstRef(
            const Ref<const MatrixXd>& x, int col) {
        return x.block(0, col, x.rows(), 1);
    }

    // Generic output parameters have no restrictions
    // and can be mixed with reference parameters.
    template <typename T>
    void assignTemplate(MatrixBase<T>& self, const Ref<const MatrixXd>& other) {
        self = other;
    }

    // Reference output parameters have some limitations
    // * cannot be resized (DEBUG compiler error)
    void assignRef(Ref<MatrixXd> self, const Ref<const MatrixXd>& other) {
        self = other;
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
    std::vector<Vector3d> aFixedCollection;
    std::vector<Vector3d> bFixedCollection;

    // Collection of fixed-capacity vectors.
    std::vector<VectorX4d> aCappedCollection;
    std::vector<VectorX4d> bCappedCollection;

    // Collection of dynamic-capacity vectors.
    std::vector<VectorXd> aDynamicCollection;
    std::vector<VectorXd> bDynamicCollection;

    // Collection of pointers to dynamic-capacity vectors.
    std::vector<std::unique_ptr<VectorXd>> aPointerCollection;
    std::vector<std::unique_ptr<VectorXd>> bPointerCollection;

    // Matrix packed with fixed-size vectors.
    Matrix3Xd aFixed;
    Matrix3Xd bFixed;

    // Matrix packed with fixed-capacity vectors.
    MatrixXX4Xd aCapped;
    MatrixXX4Xd bCapped;

    // Matrix packed with dynamic-capacity vectors.
    MatrixXd aDynamic;
    MatrixXd bDynamic;

    MatrixXd actual;
    MatrixXd expected;

    boost::timer::cpu_timer timer;
};

TEST_F(TestEigen, sizeof) {
    auto vectorDataSize = 3 * sizeof(double);
    auto matrixDataSize = aSize * vectorDataSize;

    // Pointer, size.
    auto dynamicVectorSize = sizeof(double*) + sizeof(size_t);

    // Pointer, rows, columns.
    auto dynamicMatrixSize = sizeof(double*) + 2 * sizeof(size_t);

    EXPECT_EQ(sizeof(Vector3d), vectorDataSize) << "expect no overhead";
    EXPECT_EQ(sizeof(VectorXd), dynamicVectorSize);
    EXPECT_EQ(sizeof(Matrix3Xd), dynamicMatrixSize - sizeof(size_t))
            << "expect no row count";
    EXPECT_EQ(sizeof(MatrixXd), dynamicMatrixSize);

    EXPECT_EQ(sizeof(Ref<VectorXd>), dynamicVectorSize + sizeof(size_t))
            << "expect added stride";
    EXPECT_EQ(sizeof(const Ref<const VectorXd>), sizeof(Ref<VectorXd>) + dynamicVectorSize)
            << "expect added object";
    EXPECT_EQ(sizeof(Block<VectorXd>), sizeof(Ref<VectorXd>) + 4 * sizeof(size_t));
    EXPECT_EQ(sizeof(const Block<const VectorXd>), sizeof(Block<VectorXd>));

    EXPECT_GE(sizeof(Ref<MatrixXd>), dynamicMatrixSize);
    EXPECT_EQ(sizeof(const Ref<const MatrixXd>), sizeof(Ref<MatrixXd>) + dynamicMatrixSize)
            << "expect added object";
    EXPECT_EQ(sizeof(Block<MatrixXd>), sizeof(Ref<MatrixXd>) + 2 * sizeof(size_t));
    EXPECT_EQ(sizeof(const Block<const MatrixXd>), sizeof(Block<MatrixXd>));

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

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"

    struct aligned_padded {
        double d1{};
        Matrix4d m{};
        double d2{};
    } a;

#pragma GCC diagnostic pop

    auto pa = std::make_unique<aligned_padded>();
    // NOLINTNEXTLINE(*-reinterpret-cast)
    auto pam = reinterpret_cast<std::ptrdiff_t>(std::addressof(pa->m));

    EXPECT_GT(sizeof(a), 18 * sizeof(double)) << "expect padding";
    EXPECT_EQ(alignof(decltype(a.m)), desiredAlignment);
    EXPECT_EQ(offsetof(aligned_padded, m) % desiredAlignment, 0);
    EXPECT_EQ(pam % desiredAlignment, 0);

    struct misaligned_unpadded {
        double d1{};
        Matrix<double, 4, 4, DontAlign> m{};
        double d2{};
    } b;

    auto pb = std::make_unique<misaligned_unpadded>();
    // NOLINTNEXTLINE(*-reinterpret-cast)
    auto pbm = reinterpret_cast<std::ptrdiff_t>(std::addressof(pb->m));

    EXPECT_EQ(sizeof(b), 18 * sizeof(double)) << "expect no padding";
    EXPECT_NE(offsetof(misaligned_unpadded, m) % desiredAlignment, 0);
    EXPECT_NE(pbm % desiredAlignment, 0);
}

TEST_F(TestEigen, outputParameters) {
    VectorX4d v1;
    Vector3d v2;
    v2.setRandom();

    EXPECT_EQ(v1.size(), 0);
    EXPECT_EQ(v2.size(), 3);
    assignTemplate(v1, v2);
    EXPECT_EQ(v1.size(), v2.size()); // template parameters can be resized
    EXPECT_EQ(v1, v2);

    VectorX4d v3;
    v3.resizeLike(v2);
    v3.setZero();
    EXPECT_NE(v3, v2);
    assignRef(v3, v2); // reference parameters cannot be resized
    EXPECT_EQ(v3, v2); // but their values can be modified
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

TEST_F(TestEigen, constRefMixed) {
    auto& a = aFixed;
    auto& b = bDynamic;
    for (int i = 0; i < repeat; i++) {
        for (auto row = 0; row < actual.rows(); row++) {
            for (auto col = 0; col < actual.cols(); col++) {
                actual(row, col) =
                        squaredDistanceConstRef(a.col(row), b.col(col));
            }
        }
    }
}

TEST_F(TestEigen, fixedRefMixed) {
    auto& a = aFixed;
    auto& b = bDynamic;
    for (int i = 0; i < repeat; i++) {
        for (auto row = 0; row < actual.rows(); row++) {
            for (auto col = 0; col < actual.cols(); col++) {
                actual(row, col) =
                        squaredDistanceFixedRef(a.col(row), b.col(col));
            }
        }
    }
}

TEST_F(TestEigen, fixedConstRefMixed) {
    auto& a = aFixed;
    auto& b = bDynamic;
    for (int i = 0; i < repeat; i++) {
        for (auto row = 0; row < actual.rows(); row++) {
            for (auto col = 0; col < actual.cols(); col++) {
                actual(row, col) =
                        squaredDistanceFixedConstRef(a.col(row), b.col(col));
            }
        }
    }
}

TEST_F(TestEigen, fixedMixedSlow) {
    // This test is expected to be slower because
    // arguments to squaredDistanceFixed() will be
    // converted to temporary fixed-size vectors

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

TEST_F(TestEigen, cappedMixedSlow) {
    // This test is expected to be slower because
    // arguments to squaredDistanceCapped() will be
    // converted to temporary fixed-capacity vectors

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

TEST_F(TestEigen, dynamicMixedSlow) {
    // This test is expected to take longer because
    // arguments to squaredDistanceDynamic() will be
    // converted to temporary dynamic-capacity vectors.
    // This will also cause dynamic memory allocation.
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

TEST_F(TestEigen, dynamicAsFixedSlow) {
    // This test is expected to take longer because
    // arguments to squaredDistanceFixed() will be
    // converted to temporary fixed-capacity vectors.

    auto& a = aFixed;
    auto& b = bDynamic;
    for (int i = 0; i < repeat; i++) { // NOLINT(*-avoid-magic-numbers)
        for (auto row = 0; row < actual.rows(); row++) {
            for (auto col = 0; col < actual.cols(); col++) {
                actual(row, col) = squaredDistanceFixed(a.col(row), b.col(col));
            }
        }
    }
}

TEST_F(TestEigen, blockTemplateMixed) {
    auto& a = aFixed;
    auto& b = bDynamic;
    for (int i = 0; i < repeat; i++) {
        for (auto row = 0; row < actual.rows(); row++) {
            for (auto col = 0; col < actual.cols(); col++) {
                actual(row, col) = squaredDistanceRef(
                        columnTemplate(a, row), columnTemplate(b, col));
            }
        }
    }
}

TEST_F(TestEigen, blockRefMixed) {
    auto& a = aFixed;
    auto& b = bDynamic;
    for (int i = 0; i < repeat; i++) {
        for (auto row = 0; row < actual.rows(); row++) {
            for (auto col = 0; col < actual.cols(); col++) {
                actual(row, col) = squaredDistanceConstRef(
                        columnConstRef(a, row), columnConstRef(b, col));
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

    EXPECT_TRUE(sameDataRef(columnRef(aDynamic, 0), aDynamic.data()));
    EXPECT_TRUE(sameDataConstRef(columnConstRef(aDynamic, 0), aDynamic.data()));

    // The next tests cause dynamic allocation for temporary objects.
    Eigen::internal::set_is_malloc_allowed(true);
    EXPECT_FALSE(sameData(aDynamic.leftCols(1), aDynamic.data()));
    EXPECT_FALSE(sameData(aDynamic.topRows(1), aDynamic.data()));
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
