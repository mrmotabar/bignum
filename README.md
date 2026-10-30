# Bignum: Arbitrary-Length Integer Arithmetic in C++

[![C++ Standard](https://img.shields.io/badge/C%2B%2B-17%2F20-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B17)
[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)
[![Tests](https://img.shields.io/badge/tests-72%2F72%20passing-brightgreen.svg)]()

A clean, modern, and header-only C++ library for arbitrary-length signed integer arithmetic.

---

## Table of Contents

- [Motivation & Overview](#motivation--overview)
- [Key Features](#key-features)
- [Design & Architecture](#design--architecture)
- [Repository Structure](#repository-structure)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Building with Make](#building-with-make)
  - [Building with CMake](#building-with-cmake)
- [Usage Examples](#usage-examples)
  - [Basic Arithmetic](#basic-arithmetic)
  - [Powers and Roots](#powers-and-roots)
  - [Stream Input/Output](#stream-inputoutput)
- [Performance & Benchmarks](#performance--benchmarks)
- [Testing Suite](#testing-suite)
- [License](#license)
- [Author](#author)

---

## Motivation & Overview

In standard C++, primitive integer types are capped at 64 bits (`unsigned long long` max $\approx 1.84 \times 10^{19}$). In competitive programming, cryptography, and numerical computation, numbers frequently exceed this hardware limit.

This project implements an arbitrary-length integer data structure (`Bignum`). It was designed with an intuitive, human-like pen-and-paper mental model: digits are represented in decimal base-10, making operations transparent to inspect, reason about, and debug, while maintaining high performance through binary search digit selection and dynamic memory scaling.

---

## Key Features

- **Header-Only**: Zero dependencies beyond the C++ standard library. Drop `include/bignum/bignum.hpp` directly into any project.
- **Arbitrary Length**: Numbers can span hundreds, thousands, or tens of thousands of digits, bounded only by available RAM.
- **Full Signed Support**: Handles positive integers, negative integers, and zero with standard sign propagation rules.
- **Rich Operator Overloading**:
  - Arithmetic: `+`, `-`, `*`, `/`, `%`, `+=`, `-=`, `*=`, `/=`, `%=`
  - Unary & Crement: `+a`, `-a`, `++a`, `a++`, `--a`, `a--`
  - Exponentiation: `a ^ n`, `power(n)` via fast binary exponentiation $O(\log n)$
  - Integer Square Root: `squareRoot()` via digit-by-digit root extraction
  - Relational: `==`, `!=`, `<`, `<=`, `>`, `>=`
  - Stream I/O: `std::cin >> a`, `std::cout << a`
- **Robust Error Handling**: Throws standard C++ exceptions (`std::invalid_argument` on invalid string formats or division by zero, `std::domain_error` on negative square roots).
- **Comprehensive Verification**: Includes 72 automated unit tests and a benchmark suite measuring latency and operations per second.

---

## Design & Architecture

The library is built around a decimal base-10 little-endian architecture:

| Design Dimension | Implementation Details |
| :--- | :--- |
| **Digit Representation** | Base-10 decimal digits (`0-9`) stored in a dynamic `std::vector<int>` (little-endian order: least significant digit at index 0). |
| **Dynamic Scaling** | Dynamic memory allocation allowing numbers to grow to thousands of digits, bounded only by available RAM. |
| **Signed Arithmetic** | Explicit boolean sign flag (`isNegative_`) handling positive values, negative values, and zero ($\mathbb{Z}$) consistently. |
| **Const Correctness** | All query methods, relational operators, and binary arithmetic operations are strictly const-qualified without modifying operands. |
| **Division Algorithm** | Long division with binary search quotient digit selection in $O(\log_{2} 10)$ per digit. |
| **Square Root** | Exact digit-by-digit integer square root extraction using paired-digit pencil-and-paper algorithm. |
| **Standard Integration** | Full stream operator support (`std::cin`, `std::cout`) and `std::string` parsing with input validation. |

---

## Repository Structure

```text
bignum/
├── include/
│   └── bignum/
│       └── bignum.hpp         # Core header-only Bignum implementation
├── src/
│   └── main.cpp               # Interactive CLI & demonstration binary
├── tests/
│   ├── test-bignum.cpp        # 72 comprehensive correctness unit tests
│   └── benchmark.cpp          # Runtime & throughput benchmark tester
├── CMakeLists.txt             # Modern CMake configuration
├── Makefile                   # Convenient CLI targets (build, test, benchmark)
├── LICENSE                    # MIT License
├── .gitignore                 # Build & OS artifact exclusions
└── README.md                  # Project documentation
```

---

## Getting Started

### Prerequisites

- A C++17 compliant compiler (`clang++` $\ge 7$ or `g++` $\ge 7$ or MSVC $\ge 2017$)
- `make` or `cmake` ($\ge 3.15$)

### Building with Make

```bash
# Build CLI, test runner, and benchmark suite
make all

# Run interactive demo
make run

# Run correctness unit test suite
make test

# Run performance benchmark suite
make benchmark

# Clean build artifacts
make clean
```

### Building with CMake

```bash
# Configure build
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Compile all targets
cmake --build build

# Run unit tests
ctest --test-dir build --output-on-failure
```

---

## Usage Examples

### Basic Arithmetic

```cpp
#include "bignum/bignum.hpp"
#include <iostream>

int main()
{
    // Construct from integers or decimal strings
    Bignum a("123456789012345678901234567890");
    Bignum b("987654321098765432109876543210");

    // Standard arithmetic
    Bignum sum = a + b;
    Bignum diff = b - a;
    Bignum product = a * b;
    Bignum quotient = b / a;
    Bignum remainder = b % a;

    std::cout << "A + B = " << sum << "\n";
    std::cout << "B - A = " << diff << "\n";
    std::cout << "A * B = " << product << "\n";
    std::cout << "B / A = " << quotient << "\n";
    std::cout << "B % A = " << remainder << "\n";

    return 0;
}
```

### Powers and Roots

```cpp
#include "bignum/bignum.hpp"
#include <iostream>

int main()
{
    // Fast binary exponentiation (2^128)
    Bignum base = 2;
    Bignum powerResult = base.power(128); // Or: base ^ 128
    std::cout << "2^128 = " << powerResult << "\n";
    std::cout << "Digits in 2^128: " << powerResult.digitsCount() << "\n";

    // Integer square root floor(sqrt(N))
    Bignum bigSquare("1524157875323883675019051998750190521"); // (1234567890123456789)^2
    std::cout << "sqrt(bigSquare) = " << bigSquare.squareRoot() << "\n";

    return 0;
}
```

### Stream Input/Output

```cpp
#include "bignum/bignum.hpp"
#include <iostream>

int main()
{
    Bignum x;
    std::cout << "Enter a large integer: ";
    if (std::cin >> x)
    {
        std::cout << "You entered: " << x << "\n";
        std::cout << "Is even: " << (x.isEven() ? "Yes" : "No") << "\n";
    }
    return 0;
}
```

---

## Performance & Benchmarks

Measured on Apple Silicon (M-series, `-O3` optimization):

| Operation | Digits | Iterations | Latency (Avg) | Throughput |
| :--- | ---: | ---: | ---: | ---: |
| **Addition (`+`)** | 100 | 1,000 | 0.0005 ms | ~2,000,000 ops/s |
| **Addition (`+`)** | 2,000 | 1,000 | 0.0074 ms | ~135,000 ops/s |
| **Addition (`+`)** | 10,000 | 200 | 0.0373 ms | ~27,000 ops/s |
| **Subtraction (`-`)** | 100 | 1,000 | 0.0006 ms | ~1,800,000 ops/s |
| **Subtraction (`-`)** | 2,000 | 1,000 | 0.0051 ms | ~195,000 ops/s |
| **Subtraction (`-`)** | 10,000 | 200 | 0.0258 ms | ~39,000 ops/s |
| **Multiplication (`*`)** | 50 | 200 | 0.0080 ms | ~125,000 ops/s |
| **Multiplication (`*`)** | 200 | 200 | 0.1561 ms | ~6,400 ops/s |
| **Multiplication (`*`)** | 1,000 | 10 | 3.1610 ms | ~316 ops/s |
| **Division (`/`)** | 100 / 50 | 50 | 0.1515 ms | ~6,600 ops/s |
| **Division (`/`)** | 600 / 300 | 15 | 3.6767 ms | ~272 ops/s |
| **Power ($2^{1024}$)** | 1 | 50 | 0.0995 ms | ~10,050 ops/s |
| **Square Root (`sqrt`)** | 50 | 50 | 0.0689 ms | ~14,500 ops/s |
| **Square Root (`sqrt`)** | 500 | 10 | 3.0464 ms | ~328 ops/s |

---

## Testing Suite

The repository contains an automated test runner in `tests/test-bignum.cpp` verifying 72 assertions across 8 functional domains:

1. **Constructors & Initializations**: zero, positive, negative, `LLONG_MIN`, string parsing, leading zeros, `-0` normalization, invalid input detection.
2. **Relational Operators**: `<, <=, >, >=, ==, !=`, positive vs positive, negative vs negative, positive vs negative, different digit lengths.
3. **Addition**: carrying across digit boundaries, positive + positive, positive + negative, commutative rules.
4. **Subtraction**: borrowing cascades, larger - smaller, smaller - larger (negative results), minus negative.
5. **Multiplication**: zero multiplication, negative signs, large arbitrary-length products up to $10^{60}$.
6. **Division & Modulo**: exact division, integer truncation with remainder, smaller / larger, division-by-zero exception validation, invariant verification $A = (A / B) \times B + (A \bmod B)$.
7. **Increment & Decrement**: prefix `++` / `--`, postfix `++` / `--`, transitioning across zero and 100.
8. **Exponentiation & Square Root**: $0^0$, $x^0$, powers of 2, small squares, non-perfect square floor truncation, large root recovery $(x^2 \to x)$, domain error on negative roots.

Run tests at any time via:
```bash
make test
```

---

## License

This project is licensed under the [MIT License](LICENSE).

---

## Author

**Mohammadreza Motabar**  
GitHub: [@mrmotabar](https://github.com/mrmotabar)
