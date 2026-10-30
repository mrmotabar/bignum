cmake_minimum_required(VERSION 3.15)
project(bignum VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -O3)
endif()

# Header-only library interface
add_library(bignum INTERFACE)
target_include_directories(bignum INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# CLI executable
add_executable(bignum_cli src/main.cpp)
target_link_libraries(bignum_cli PRIVATE bignum)

# Correctness test executable
add_executable(test_bignum tests/test-bignum.cpp)
target_link_libraries(test_bignum PRIVATE bignum)

# Benchmark executable
add_executable(benchmark tests/benchmark.cpp)
target_link_libraries(benchmark PRIVATE bignum)

enable_testing()
add_test(NAME bignum_correctness_test COMMAND test_bignum)
