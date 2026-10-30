/**
 * @file main.cpp
 * @author Mohammadreza Motabar
 * @brief Demonstration and interactive CLI for the Bignum library.
 */

#include "bignum/bignum.hpp"

#include <iostream>
#include <string>
#include <vector>

#if defined(_WIN32)
#include <io.h>
#define ISATTY _isatty
#define FILENO _fileno
#else
#include <unistd.h>
#define ISATTY isatty
#define FILENO fileno
#endif

void printHeader()
{
    std::cout << "========================================================\n";
    std::cout << "   Bignum: Arbitrary-Length Integer Arithmetic in C++\n";
    std::cout << "========================================================\n\n";
}

void runDemo()
{
    std::cout << "[1] Basic Arithmetic Operations Demo:\n";
    Bignum firstNumber("123456789012345678901234567890");
    Bignum secondNumber("987654321098765432109876543210");

    std::cout << "  A = " << firstNumber << "\n";
    std::cout << "  B = " << secondNumber << "\n\n";

    std::cout << "  A + B = " << (firstNumber + secondNumber) << "\n";
    std::cout << "  B - A = " << (secondNumber - firstNumber) << "\n";
    std::cout << "  A - B = " << (firstNumber - secondNumber) << " (signed)\n";
    std::cout << "  A * B = " << (firstNumber * secondNumber) << "\n";
    std::cout << "  B / A = " << (secondNumber / firstNumber) << "\n";
    std::cout << "  B % A = " << (secondNumber % firstNumber) << "\n\n";

    std::cout << "[2] Powers and Roots Demo:\n";
    Bignum baseValue = 2;
    Bignum powerResult = baseValue.power(128);
    std::cout << "  2^128 = " << powerResult << "\n";
    std::cout << "  Digits in 2^128: " << powerResult.digitsCount() << "\n\n";

    Bignum perfectSquare = Bignum("1234567890123456789") * Bignum("1234567890123456789");
    std::cout << "  N = (1234567890123456789)^2\n";
    std::cout << "  N = " << perfectSquare << "\n";
    std::cout << "  sqrt(N) = " << perfectSquare.squareRoot() << "\n\n";

    std::cout << "[3] Comparisons and Properties:\n";
    std::cout << "  A == B: " << (firstNumber == secondNumber ? "true" : "false") << "\n";
    std::cout << "  A <  B: " << (firstNumber < secondNumber ? "true" : "false") << "\n";
    std::cout << "  A is even: " << (firstNumber.isEven() ? "true" : "false") << "\n";
    std::cout << "  B is even: " << (secondNumber.isEven() ? "true" : "false") << "\n\n";
}

int main(int argc, char *argv[])
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    // If a numerical argument is provided on CLI, compute square root directly
    if (argc > 1)
    {
        try
        {
            Bignum inputNumber(argv[1]);
            std::cout << inputNumber.squareRoot() << "\n";
            return 0;
        }
        catch (const std::exception &exception)
        {
            std::cerr << "Error: " << exception.what() << "\n";
            return 1;
        }
    }

    // If input is piped (non-interactive stdin), read a single number and print its square root
    if (!ISATTY(FILENO(stdin)))
    {
        std::string inputToken;
        if (std::cin >> inputToken)
        {
            try
            {
                Bignum inputNumber(inputToken);
                std::cout << inputNumber.squareRoot() << "\n";
            }
            catch (const std::exception &exception)
            {
                std::cerr << "Error: " << exception.what() << "\n";
                return 1;
            }
        }
        return 0;
    }

    // Interactive mode
    printHeader();
    runDemo();

    std::cout << "For automated unit tests:  make test\n";
    std::cout << "For runtime benchmarks:    make benchmark\n";

    return 0;
}
