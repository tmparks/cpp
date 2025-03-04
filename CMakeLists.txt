cmake_minimum_required(VERSION 3.30) # Ubuntu 24.10
project(cpp VERSION 2025.02)

find_package(Boost CONFIG REQUIRED COMPONENTS timer)
find_package(Eigen3 CONFIG REQUIRED)
find_package(Microsoft.GSL CONFIG REQUIRED)
find_package(OpenMP REQUIRED)

# Default test options
list(APPEND CMAKE_CTEST_ARGUMENTS
    --output-on-failure
    --output-junit ${CMAKE_BINARY_DIR}/results/test_detail.xml
)
enable_testing()
find_package(GTest REQUIRED)

# Add run-time instrumentation for Debug builds
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        # See [Source Based Code Coverage](https://clang.llvm.org/docs/SourceBasedCodeCoverage.html)
        set(LLVM_COV_BINARIES TestRunner)
        add_custom_target(coverage
            COMMAND llvm-profdata merge
                --sparse=true
                --output=merged.profdata
                *.profraw
            COMMAND llvm-cov show
                -format=html
                -output-dir=results
                -instr-profile=merged.profdata
                ${LLVM_COV_BINARIES}
            COMMAND llvm-cov report
                -show-branch-summary=false
                -instr-profile=merged.profdata
                ${LLVM_COV_BINARIES}
            COMMAND mv results/index.html results/coverage_details.html
            COMMAND rm *.profraw *.profdata
        )
        set(INSTRUMENTATION_OPTIONS
            -fprofile-instr-generate
            -fcoverage-mapping
            -fsanitize=address
            -fsanitize=leak
            -fsanitize=undefined
        )
        add_compile_definitions(LIBCXX_ENABLE_DEBUG_MODE)
    else() # Assume GNU-compatible compiler
        # See [Out-of-Source Builds with CMake](https://gcovr.com/en/stable/cookbook.html#out-of-source-builds-with-cmake)
        add_custom_target(coverage
            COMMAND gcovr
                --txt
                --xml results/
                --html-details results/
                --root ${CMAKE_SOURCE_DIR}
                ${CMAKE_BINARY_DIR}
        )
        set(INSTRUMENTATION_OPTIONS
            --coverage
            -fsanitize=address
            -fsanitize=leak
            -fsanitize=undefined
        )
        add_compile_definitions(_GLIBCXX_DEBUG _GLIBCXX_DEBUG_BACKTRACE)
        link_libraries(stdc++exp)
    endif() # CMAKE_CXX_COMPILER_ID
    add_compile_options(${INSTRUMENTATION_OPTIONS})
    add_link_options(${INSTRUMENTATION_OPTIONS})
endif() # CMAKE_BUILD_TYPE

# Default build options
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Needed by clang-tidy (see below)
if(NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 20)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
    -march=native
    -mtune=native
)

################################################################################

add_executable(TestRunner
    Array.cpp
    Shape.cpp
    StretchableArray.cpp
    TestArray.cpp
    TestConstPointer.cpp
    TestConstant.cpp
    TestCopyElision.cpp
    TestEigen.cpp
    TestLockable.cpp
    TestPointerMap.cpp
    TestPointerVector.cpp
    TestRawRef.cpp
    TestShape.cpp
    TestSharedCloneable.cpp
    TestSharedFromThis.cpp
    TestSharedRef.cpp
    TestStretchableArray.cpp
    TestStrongType.cpp
    TestUniqueRef.cpp
    TestVerbose.cpp
    TestVirtualDataPtr.cpp
    TestVirtualDataRef.cpp
    TestZip.cpp
)

target_link_libraries(TestRunner PRIVATE
    Boost::timer
    Eigen3::Eigen
    Microsoft.GSL::GSL
    OpenMP::OpenMP_CXX
    GTest::gtest
    GTest::gtest_main)
gtest_discover_tests(TestRunner
    PROPERTIES ENVIRONMENT LLVM_PROFILE_FILE=llvm-%p.profraw
)

################################################################################

# Perform static analysis on all source files.
# Because setting CMAKE_CXX_CLANG_TIDY significantly slows compilation,
# we define a custom target to run clang-tidy.
# See [Clang-Tidy](https://clang.llvm.org/extra/clang-tidy/)
# See [How to use clang-tidy in CMake](https://stackoverflow.com/questions/67999371/how-to-use-clang-tidy-in-cmake)
add_custom_target(check COMMAND run-clang-tidy -quiet -p ${CMAKE_BINARY_DIR})
