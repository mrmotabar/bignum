/**
 * @file benchmark.cpp
 * @author Mohammadreza Motabar
 * @brief Performance and runtime benchmark tester for the Bignum library.
 */

#include "bignum/bignum.hpp"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <utility>
#include <vector>

namespace
{

std::string generateRandomNumericString(size_t numberOfDigits, std::mt19937_64 &randomEngine)
{
    if (numberOfDigits == 0)
    {
        return "0";
    }

    std::uniform_int_distribution<int> nonZeroDigitDistribution(1, 9);
    std::uniform_int_distribution<int> anyDigitDistribution(0, 9);

    std::string result;
    result.reserve(numberOfDigits);
    result.push_back(static_cast<char>('0' + nonZeroDigitDistribution(randomEngine)));

    for (size_t i = 1; i < numberOfDigits; ++i)
    {
        result.push_back(static_cast<char>('0' + anyDigitDistribution(randomEngine)));
    }

    return result;
}

template <typename Function>
double measureExecutionTimeMilliseconds(Function &&function, int iterations = 1)
{
    auto startTime = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i)
    {
        function();
    }
    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsedMilliseconds = endTime - startTime;
    return elapsedMilliseconds.count() / iterations;
}

void printBenchmarkRow(const std::string &operationName, size_t digitSize, int iterations, double averageTimeMilliseconds)
{
    std::cout << "| " << std::left << std::setw(20) << operationName
              << " | " << std::right << std::setw(12) << digitSize
              << " | " << std::right << std::setw(12) << iterations
              << " | " << std::right << std::setw(14) << std::fixed << std::setprecision(4) << averageTimeMilliseconds << " ms"
              << " | " << std::right << std::setw(12) << std::fixed << std::setprecision(1)
              << (averageTimeMilliseconds > 0.0 ? (1000.0 / averageTimeMilliseconds) : 0.0) << " ops/s"
              << " |\n";
}

} // namespace

int main()
{
    std::cout << "========================================================================================\n";
    std::cout << "                             Bignum Performance Benchmark                              \n";
    std::cout << "========================================================================================\n\n";

    std::mt19937_64 randomEngine(42);

    std::cout << "| " << std::left << std::setw(20) << "Operation"
              << " | " << std::right << std::setw(12) << "Digits"
              << " | " << std::right << std::setw(12) << "Iterations"
              << " | " << std::right << std::setw(17) << "Avg Time (ms)"
              << " | " << std::right << std::setw(18) << "Throughput"
              << " |\n";
    std::cout << "|:---------------------|-------------:|-------------:|------------------:|-------------------:|\n";

    // 1. Addition Benchmarks
    std::vector<size_t> additionSizes = {100, 500, 2000, 10000};
    for (size_t size : additionSizes)
    {
        Bignum operandA(generateRandomNumericString(size, randomEngine));
        Bignum operandB(generateRandomNumericString(size, randomEngine));
        volatile size_t preventOptimize = 0;

        int iterations = (size <= 2000) ? 1000 : 200;
        double timeMilliseconds = measureExecutionTimeMilliseconds([&]() {
            Bignum result = operandA + operandB;
            preventOptimize += result.digitsCount();
        }, iterations);

        printBenchmarkRow("Addition (+)", size, iterations, timeMilliseconds);
    }

    // 2. Subtraction Benchmarks
    std::vector<size_t> subtractionSizes = {100, 500, 2000, 10000};
    for (size_t size : subtractionSizes)
    {
        Bignum operandA(generateRandomNumericString(size, randomEngine));
        Bignum operandB(generateRandomNumericString(size, randomEngine));
        volatile size_t preventOptimize = 0;

        int iterations = (size <= 2000) ? 1000 : 200;
        double timeMilliseconds = measureExecutionTimeMilliseconds([&]() {
            Bignum result = operandA - operandB;
            preventOptimize += result.digitsCount();
        }, iterations);

        printBenchmarkRow("Subtraction (-)", size, iterations, timeMilliseconds);
    }

    // 3. Multiplication Benchmarks
    std::vector<size_t> multiplicationSizes = {50, 200, 500, 1000};
    for (size_t size : multiplicationSizes)
    {
        Bignum operandA(generateRandomNumericString(size, randomEngine));
        Bignum operandB(generateRandomNumericString(size, randomEngine));
        volatile size_t preventOptimize = 0;

        int iterations = (size <= 200) ? 200 : (size <= 500 ? 50 : 10);
        double timeMilliseconds = measureExecutionTimeMilliseconds([&]() {
            Bignum result = operandA * operandB;
            preventOptimize += result.digitsCount();
        }, iterations);

        printBenchmarkRow("Multiplication (*)", size, iterations, timeMilliseconds);
    }

    // 4. Division Benchmarks
    std::vector<std::pair<size_t, size_t>> divisionSizes = {{100, 50}, {300, 150}, {600, 300}};
    for (auto &[dividendSize, divisorSize] : divisionSizes)
    {
        Bignum dividend(generateRandomNumericString(dividendSize, randomEngine));
        Bignum divisor(generateRandomNumericString(divisorSize, randomEngine));
        volatile size_t preventOptimize = 0;

        int iterations = (dividendSize <= 300) ? 50 : 15;
        double timeMilliseconds = measureExecutionTimeMilliseconds([&]() {
            Bignum quotient = dividend / divisor;
            preventOptimize += quotient.digitsCount();
        }, iterations);

        std::string label = "Div (" + std::to_string(dividendSize) + "/" + std::to_string(divisorSize) + ")";
        printBenchmarkRow(label, dividendSize, iterations, timeMilliseconds);
    }

    // 5. Exponentiation Benchmarks
    {
        Bignum base(2);
        volatile size_t preventOptimize = 0;
        double timeMilliseconds = measureExecutionTimeMilliseconds([&]() {
            Bignum result = base.power(1024);
            preventOptimize += result.digitsCount();
        }, 50);
        printBenchmarkRow("Power (2^1024)", 1, 50, timeMilliseconds);
    }

    // 6. Square Root Benchmarks
    std::vector<size_t> sqrtSizes = {50, 200, 500};
    for (size_t size : sqrtSizes)
    {
        Bignum input(generateRandomNumericString(size, randomEngine));
        volatile size_t preventOptimize = 0;

        int iterations = (size <= 200) ? 50 : 10;
        double timeMilliseconds = measureExecutionTimeMilliseconds([&]() {
            Bignum root = input.squareRoot();
            preventOptimize += root.digitsCount();
        }, iterations);

        printBenchmarkRow("Square Root (sqrt)", size, iterations, timeMilliseconds);
    }

    std::cout << "\nBenchmark completed successfully.\n";
    return 0;
}
