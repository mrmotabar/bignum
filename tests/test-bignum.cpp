/**
 * @file test-bignum.cpp
 * @author Mohammadreza Motabar
 * @brief Comprehensive correctness test suite for the Bignum library.
 */

#include "bignum/bignum.hpp"

#include <cassert>
#include <climits>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace
{

int totalTestsExecuted = 0;
int totalTestsPassed = 0;

void assertCondition(bool condition, const std::string &testName, const std::string &details = "")
{
    totalTestsExecuted++;
    if (condition)
    {
        totalTestsPassed++;
        std::cout << "  [PASS] " << testName << "\n";
    }
    else
    {
        std::cerr << "  [FAIL] " << testName;
        if (!details.empty())
        {
            std::cerr << " (" << details << ")";
        }
        std::cerr << "\n";
    }
}

void testConstructorsAndQueries()
{
    std::cout << "\n--- Testing Constructors and Queries ---\n";

    Bignum zeroDefault;
    assertCondition(zeroDefault.isZero(), "Default constructor initializes to zero");
    assertCondition(zeroDefault.toString() == "0", "Default constructor string representation is '0'");
    assertCondition(!zeroDefault.isNegative(), "Zero is not negative");
    assertCondition(zeroDefault.digitsCount() == 1, "Zero has 1 digit");
    assertCondition(zeroDefault.isEven(), "Zero is even");

    Bignum positiveInt(12345);
    assertCondition(positiveInt.toString() == "12345", "Construct from positive integer");
    assertCondition(positiveInt.digitsCount() == 5, "Positive int digit count");
    assertCondition(!positiveInt.isNegative(), "Positive int is not negative");
    assertCondition(positiveInt.isOdd(), "12345 is odd");

    Bignum negativeInt(-9876);
    assertCondition(negativeInt.toString() == "-9876", "Construct from negative integer");
    assertCondition(negativeInt.isNegative(), "Negative int isNegative is true");
    assertCondition(negativeInt.sign() == -1, "Negative int sign is -1");
    assertCondition(negativeInt.isEven(), "-9876 is even");

    Bignum minLongLong(LLONG_MIN);
    std::string minLongLongString = std::to_string(LLONG_MIN);
    assertCondition(minLongLong.toString() == minLongLongString, "Construct from LLONG_MIN");

    Bignum fromStringPositive("9876543210123456789");
    assertCondition(fromStringPositive.toString() == "9876543210123456789", "Construct from string (large positive)");

    Bignum fromStringNegative("-1234567890123456789");
    assertCondition(fromStringNegative.toString() == "-1234567890123456789", "Construct from string (large negative)");

    Bignum withLeadingZeros("000042");
    assertCondition(withLeadingZeros.toString() == "42", "Leading zeros are stripped properly");

    Bignum negativeZero("-0");
    assertCondition(negativeZero.isZero() && !negativeZero.isNegative(), "'-0' normalizes to positive zero");

    bool exceptionThrown = false;
    try
    {
        Bignum invalid("123a45");
    }
    catch (const std::invalid_argument &)
    {
        exceptionThrown = true;
    }
    assertCondition(exceptionThrown, "Invalid string throws std::invalid_argument");
}

void testComparisons()
{
    std::cout << "\n--- Testing Relational Operators ---\n";

    Bignum a("100");
    Bignum b("200");
    Bignum c("100");
    Bignum negativeA("-100");
    Bignum negativeB("-200");

    assertCondition(a == c, "Equality: 100 == 100");
    assertCondition(a != b, "Inequality: 100 != 200");
    assertCondition(a < b, "Less than: 100 < 200");
    assertCondition(b > a, "Greater than: 200 > 100");
    assertCondition(a <= c, "Less or equal: 100 <= 100");
    assertCondition(a >= c, "Greater or equal: 100 >= 100");

    assertCondition(negativeA > negativeB, "Negative comparison: -100 > -200");
    assertCondition(negativeB < negativeA, "Negative comparison: -200 < -100");
    assertCondition(negativeA < a, "Negative less than positive: -100 < 100");
    assertCondition(a > negativeA, "Positive greater than negative: 100 > -100");

    Bignum shorterNumber("9999");
    Bignum longerNumber("10000");
    assertCondition(shorterNumber < longerNumber, "Digit length comparison: 9999 < 10000");
    assertCondition(longerNumber > shorterNumber, "Digit length comparison: 10000 > 9999");
}

void testAdditionAndSubtraction()
{
    std::cout << "\n--- Testing Addition and Subtraction ---\n";

    Bignum a("99999999999999999999");
    Bignum b("1");
    Bignum sum = a + b;
    assertCondition(sum.toString() == "100000000000000000000", "Addition with carry propagation");

    Bignum x("12345");
    Bignum y("67890");
    assertCondition((x + y).toString() == "80235", "Standard addition");

    Bignum positiveValue("100");
    Bignum negativeValue("-30");
    assertCondition((positiveValue + negativeValue).toString() == "70", "Addition of positive and negative");
    assertCondition((negativeValue + positiveValue).toString() == "70", "Commutative addition of positive and negative");

    Bignum smaller("30");
    Bignum larger("100");
    assertCondition((smaller - larger).toString() == "-70", "Subtraction producing negative result");
    assertCondition((larger - smaller).toString() == "70", "Standard subtraction");

    Bignum cascadingBorrow("1000000");
    Bignum one("1");
    assertCondition((cascadingBorrow - one).toString() == "999999", "Cascading borrows in subtraction");

    Bignum differenceWithSelf = a - a;
    assertCondition(differenceWithSelf.isZero(), "A - A is zero");

    Bignum minusNegative = larger - negativeValue;
    assertCondition(minusNegative.toString() == "130", "100 - (-30) = 130");
}

void testMultiplication()
{
    std::cout << "\n--- Testing Multiplication ---\n";

    Bignum a("123456789");
    Bignum b("987654321");
    Bignum product = a * b;
    assertCondition(product.toString() == "121932631112635269", "Large integer multiplication");

    Bignum zero(0);
    assertCondition((a * zero).isZero(), "Multiplication by zero yields zero");

    Bignum negativeA("-12345");
    Bignum positiveB("10");
    assertCondition((negativeA * positiveB).toString() == "-123450", "Negative * Positive");
    assertCondition((negativeA * Bignum("-10")).toString() == "123450", "Negative * Negative");

    Bignum largePowerOfTen = Bignum("1" + std::string(30, '0')) * Bignum("1" + std::string(30, '0'));
    assertCondition(largePowerOfTen.toString() == ("1" + std::string(60, '0')), "10^30 * 10^30 = 10^60");
}

void testDivisionAndModulo()
{
    std::cout << "\n--- Testing Division and Modulo ---\n";

    Bignum dividend("1000");
    Bignum divisor("7");

    Bignum quotient = dividend / divisor;
    Bignum remainder = dividend % divisor;

    assertCondition(quotient.toString() == "142", "1000 / 7 = 142");
    assertCondition(remainder.toString() == "6", "1000 % 7 = 6");
    assertCondition((quotient * divisor + remainder) == dividend, "Division invariant: A = (A/B)*B + A%B");

    Bignum exactDividend("1000000");
    Bignum exactDivisor("1000");
    assertCondition((exactDividend / exactDivisor).toString() == "1000", "Exact division");
    assertCondition((exactDividend % exactDivisor).isZero(), "Exact division remainder is 0");

    Bignum smaller("5");
    Bignum larger("100");
    assertCondition((smaller / larger).isZero(), "Smaller / Larger = 0");
    assertCondition((smaller % larger) == smaller, "Smaller % Larger = Smaller");

    bool divisionByZeroCaught = false;
    try
    {
        Bignum invalid = dividend / Bignum(0);
    }
    catch (const std::invalid_argument &)
    {
        divisionByZeroCaught = true;
    }
    assertCondition(divisionByZeroCaught, "Division by zero throws std::invalid_argument");
}

void testIncrementAndDecrement()
{
    std::cout << "\n--- Testing Increment and Decrement ---\n";

    Bignum value("99");
    assertCondition((++value).toString() == "100", "Prefix ++99 -> 100");
    assertCondition((value++).toString() == "100", "Postfix 100++ returns 100");
    assertCondition(value.toString() == "101", "Value after postfix ++ is 101");

    assertCondition((--value).toString() == "100", "Prefix --101 -> 100");
    assertCondition((value--).toString() == "100", "Postfix 100-- returns 100");
    assertCondition(value.toString() == "99", "Value after postfix -- is 99");

    Bignum zero(0);
    assertCondition((--zero).toString() == "-1", "Decrementing 0 gives -1");
}

void testPowersAndSquareRoot()
{
    std::cout << "\n--- Testing Exponentiation and Square Root ---\n";

    Bignum two(2);
    assertCondition((two ^ 10).toString() == "1024", "2^10 = 1024");
    assertCondition((two ^ 32).toString() == "4294967296", "2^32 = 4294967296");
    assertCondition((two ^ 0).toString() == "1", "2^0 = 1");

    Bignum zero(0);
    assertCondition((zero ^ 5).isZero(), "0^5 = 0");

    // Square root tests
    Bignum hundred(100);
    assertCondition(hundred.squareRoot().toString() == "10", "sqrt(100) = 10");

    Bignum zeroValue(0);
    assertCondition(zeroValue.squareRoot().isZero(), "sqrt(0) = 0");

    Bignum oneValue(1);
    assertCondition(oneValue.squareRoot().toString() == "1", "sqrt(1) = 1");

    Bignum nonPerfect(99);
    assertCondition(nonPerfect.squareRoot().toString() == "9", "floor(sqrt(99)) = 9");

    Bignum largeBase("1234567890123456");
    Bignum largeSquare = largeBase * largeBase;
    assertCondition(largeSquare.squareRoot() == largeBase, "sqrt((1234567890123456)^2) recovers exact root");

    bool negativeRootException = false;
    try
    {
        Bignum negativeNumber("-25");
        (void)negativeNumber.squareRoot();
    }
    catch (const std::domain_error &)
    {
        negativeRootException = true;
    }
    assertCondition(negativeRootException, "Square root of negative throws std::domain_error");
}

void testStreamIO()
{
    std::cout << "\n--- Testing Stream Input/Output ---\n";

    std::stringstream outputStream;
    Bignum number("12345678901234567890");
    outputStream << number;
    assertCondition(outputStream.str() == "12345678901234567890", "Stream output operator <<");

    std::stringstream inputStream("-98765432109876543210");
    Bignum parsedNumber;
    inputStream >> parsedNumber;
    assertCondition(parsedNumber.toString() == "-98765432109876543210", "Stream input operator >>");
}

} // namespace

int main()
{
    std::cout << "========================================================\n";
    std::cout << "        Running Bignum Correctness Test Suite           \n";
    std::cout << "========================================================\n";

    testConstructorsAndQueries();
    testComparisons();
    testAdditionAndSubtraction();
    testMultiplication();
    testDivisionAndModulo();
    testIncrementAndDecrement();
    testPowersAndSquareRoot();
    testStreamIO();

    std::cout << "\n========================================================\n";
    std::cout << " Test Results: " << totalTestsPassed << " / " << totalTestsExecuted << " passed.\n";
    std::cout << "========================================================\n";

    if (totalTestsPassed == totalTestsExecuted)
    {
        std::cout << " ALL TESTS PASSED SUCCESSFULLY! \n";
        return 0;
    }
    else
    {
        std::cerr << " SOME TESTS FAILED! \n";
        return 1;
    }
}
