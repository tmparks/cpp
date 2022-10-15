cmake_minimum_required(VERSION 3.22)
project(cpp VERSION 2022.09)

# Default test options
list(APPEND CMAKE_CTEST_ARGUMENTS
    --output-on-failure
    --output-junit ${CMAKE_BINARY_DIR}/results/test_detail.xml
)
include(CTest)
find_package(GTest REQUIRED)

# Debug options
set(CMAKE_CXX_FLAGS_DEBUG --coverage)
if(CMAKE_BUILD_TYPE STREQUAL Debug)
    # https://gcovr.com/en/stable/cookbook.html#out-of-source-builds-with-cmake
    add_custom_target(coverage
        COMMAND gcovr
            --txt
            --xml results/
            --html-details results/
            --root ${CMAKE_SOURCE_DIR}
            ${CMAKE_BINARY_DIR}
    )
    set(CMAKE_CXX_CLANG_TIDY clang-tidy)
endif()

# Default build options
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_compile_options(-Werror -Wall -Wextra -Wpedantic)

################################################################################

add_executable(TestRunner
    Array.cpp
    CopyElision.cpp
    StretchableArray.cpp
    Verbose.cpp
    VirtualDataPtr.cpp
    VirtualDataRef.cpp    
)

target_link_libraries(TestRunner GTest::gtest GTest::gtest_main)
gtest_discover_tests(TestRunner)
