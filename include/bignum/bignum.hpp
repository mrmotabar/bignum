/**
 * @file bignum.hpp
 * @author Mohammadreza Motabar
 * @brief Arbitrary-length integer arithmetic library in C++.
 * @version 1.0.0
 * 
 * Provides arbitrary-length signed integer support with natural decimal
 * arithmetic, operator overloads, binary search division/root algorithms,
 * and high-performance stream integration.
 */

#ifndef BIGNUM_BIGNUM_HPP
#define BIGNUM_BIGNUM_HPP

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace bignum
{

/**
 * @class Bignum
 * @brief Represents an arbitrary-length signed integer.
 * 
 * Digits are stored in base-10 (least significant digit first) inside a
 * dynamic vector, supporting numbers of practically unbounded length.
 */
class Bignum
{
public:
    /// Default numerical base for internal digit representation.
    static constexpr int DEFAULT_BASE = 10;

    // =========================================================================
    // Constructors and Destructor
    // =========================================================================

    /**
     * @brief Constructs a Bignum with a default initial value of zero.
     */
    Bignum() : digits_{0}, isNegative_{false} {}

    /**
     * @brief Constructs a Bignum from a 64-bit signed integer.
     * @param value The numerical value to initialize with.
     */
    Bignum(long long value) : isNegative_{false}
    {
        if (value < 0)
        {
            isNegative_ = true;
            // Handle LLONG_MIN safely by handling the least significant digit first
            long long remainder = -(value % DEFAULT_BASE);
            digits_.push_back(static_cast<int>(remainder));
            value = -(value / DEFAULT_BASE);
        }
        else if (value == 0)
        {
            digits_.push_back(0);
            return;
        }

        while (value > 0)
        {
            digits_.push_back(static_cast<int>(value % DEFAULT_BASE));
            value /= DEFAULT_BASE;
        }
    }

    /**
     * @brief Constructs a Bignum from a 32-bit signed integer.
     * @param value The numerical value to initialize with.
     */
    Bignum(int value) : Bignum(static_cast<long long>(value)) {}

    /**
     * @brief Constructs a Bignum from a string representation.
     * @param valueString String containing an optional leading '+' or '-' followed by decimal digits.
     * @throws std::invalid_argument If the string is empty or contains non-digit characters.
     */
    explicit Bignum(const std::string &valueString)
    {
        fromString(valueString);
    }

    /**
     * @brief Constructs a Bignum from a C-string representation.
     * @param valueString Null-terminated string containing decimal digits.
     * @throws std::invalid_argument If the pointer is null or string is invalid.
     */
    Bignum(const char *valueString)
    {
        if (valueString == nullptr)
        {
            throw std::invalid_argument("Cannot construct Bignum from null pointer");
        }
        fromString(std::string(valueString));
    }

    /// Default copy constructor
    Bignum(const Bignum &other) = default;

    /// Default move constructor
    Bignum(Bignum &&other) noexcept = default;

    /// Default copy assignment operator
    Bignum &operator=(const Bignum &other) = default;

    /// Default move assignment operator
    Bignum &operator=(Bignum &&other) noexcept = default;

    /// Default destructor
    ~Bignum() = default;

    // =========================================================================
    // State and Query Methods
    // =========================================================================

    /**
     * @brief Checks if this number is strictly negative.
     * @return true if negative, false otherwise.
     */
    [[nodiscard]] bool isNegative() const noexcept
    {
        return isNegative_;
    }

    /**
     * @brief Checks if this number is equal to zero.
     * @return true if zero, false otherwise.
     */
    [[nodiscard]] bool isZero() const noexcept
    {
        return digits_.size() == 1 && digits_[0] == 0;
    }

    /**
     * @brief Checks if this number is even.
     * @return true if even, false if odd.
     */
    [[nodiscard]] bool isEven() const noexcept
    {
        return digits_.empty() || (digits_[0] % 2 == 0);
    }

    /**
     * @brief Checks if this number is odd.
     * @return true if odd, false if even.
     */
    [[nodiscard]] bool isOdd() const noexcept
    {
        return !isEven();
    }

    /**
     * @brief Returns the total number of decimal digits in this number.
     * @return The digit count.
     */
    [[nodiscard]] size_t digitsCount() const noexcept
    {
        return digits_.size();
    }

    /**
     * @brief Returns the signum of this number (-1, 0, or 1).
     * @return -1 if negative, 0 if zero, 1 if positive.
     */
    [[nodiscard]] int sign() const noexcept
    {
        if (isZero())
        {
            return 0;
        }
        return isNegative_ ? -1 : 1;
    }

    /**
     * @brief Retrieves the digit at a specific zero-indexed position from the right.
     * @param index The 0-based position from the least significant digit.
     * @return The digit [0-9], or 0 if index is beyond the number's length.
     */
    [[nodiscard]] int digitAt(size_t index) const noexcept
    {
        if (index < digits_.size())
        {
            return digits_[index];
        }
        return 0;
    }

    // =========================================================================
    // Conversions and String Formatting
    // =========================================================================

    /**
     * @brief Converts the Bignum to its standard decimal string representation.
     * @return Formatted string (e.g. "-12345" or "42").
     */
    [[nodiscard]] std::string toString() const
    {
        if (isZero())
        {
            return "0";
        }
        std::string result;
        result.reserve(digits_.size() + (isNegative_ ? 1 : 0));
        if (isNegative_)
        {
            result.push_back('-');
        }
        for (auto it = digits_.rbegin(); it != digits_.rend(); ++it)
        {
            result.push_back(static_cast<char>('0' + *it));
        }
        return result;
    }

    /**
     * @brief Prints the Bignum to an output stream.
     * @param stream The destination output stream (defaults to std::cout).
     */
    void print(std::ostream &stream = std::cout) const
    {
        stream << toString();
    }


    // =========================================================================
    // Unary Operators
    // =========================================================================

    /**
     * @brief Unary plus operator.
     * @return A copy of the current Bignum.
     */
    Bignum operator+() const
    {
        return *this;
    }

    /**
     * @brief Unary minus (negation) operator.
     * @return The negated Bignum.
     */
    Bignum operator-() const
    {
        if (isZero())
        {
            return *this;
        }
        Bignum result = *this;
        result.isNegative_ = !result.isNegative_;
        return result;
    }

    // =========================================================================
    // Comparison Operators
    // =========================================================================

    [[nodiscard]] bool operator==(const Bignum &other) const noexcept
    {
        if (isZero() && other.isZero())
        {
            return true;
        }
        return isNegative_ == other.isNegative_ && digits_ == other.digits_;
    }

    [[nodiscard]] bool operator!=(const Bignum &other) const noexcept
    {
        return !(*this == other);
    }

    [[nodiscard]] bool operator<(const Bignum &other) const noexcept
    {
        if (isZero() && other.isZero())
        {
            return false;
        }
        if (isNegative_ != other.isNegative_)
        {
            return isNegative_;
        }
        int absoluteComparison = compareAbsolute(*this, other);
        return isNegative_ ? (absoluteComparison > 0) : (absoluteComparison < 0);
    }

    [[nodiscard]] bool operator<=(const Bignum &other) const noexcept
    {
        return !(other < *this);
    }

    [[nodiscard]] bool operator>(const Bignum &other) const noexcept
    {
        return other < *this;
    }

    [[nodiscard]] bool operator>=(const Bignum &other) const noexcept
    {
        return !(*this < other);
    }

    // Overloads with long long
    [[nodiscard]] bool operator==(long long other) const { return *this == Bignum(other); }
    [[nodiscard]] bool operator!=(long long other) const { return *this != Bignum(other); }
    [[nodiscard]] bool operator<(long long other) const { return *this < Bignum(other); }
    [[nodiscard]] bool operator<=(long long other) const { return *this <= Bignum(other); }
    [[nodiscard]] bool operator>(long long other) const { return *this > Bignum(other); }
    [[nodiscard]] bool operator>=(long long other) const { return *this >= Bignum(other); }

    // =========================================================================
    // Addition Operators
    // =========================================================================

    /**
     * @brief Addition of two Bignum instances.
     * @param addend The value to add.
     * @return Sum of *this and addend.
     */
    Bignum operator+(const Bignum &addend) const
    {
        if (isNegative_ == addend.isNegative_)
        {
            Bignum result = addAbsolute(*this, addend);
            result.isNegative_ = isNegative_;
            result.normalize();
            return result;
        }

        // Signs are different: perform magnitude subtraction
        int comparison = compareAbsolute(*this, addend);
        if (comparison == 0)
        {
            return Bignum(0);
        }
        if (comparison > 0)
        {
            Bignum result = subtractAbsolute(*this, addend);
            result.isNegative_ = isNegative_;
            result.normalize();
            return result;
        }
        else
        {
            Bignum result = subtractAbsolute(addend, *this);
            result.isNegative_ = addend.isNegative_;
            result.normalize();
            return result;
        }
    }

    Bignum operator+(long long addend) const
    {
        return *this + Bignum(addend);
    }

    Bignum &operator+=(const Bignum &addend)
    {
        *this = *this + addend;
        return *this;
    }

    Bignum &operator+=(long long addend)
    {
        *this = *this + Bignum(addend);
        return *this;
    }

    // =========================================================================
    // Subtraction Operators
    // =========================================================================

    /**
     * @brief Subtraction of two Bignum instances.
     * @param subtrahend The value to subtract.
     * @return Difference of *this and subtrahend.
     */
    Bignum operator-(const Bignum &subtrahend) const
    {
        return *this + (-subtrahend);
    }

    Bignum operator-(long long subtrahend) const
    {
        return *this - Bignum(subtrahend);
    }

    Bignum &operator-=(const Bignum &subtrahend)
    {
        *this = *this - subtrahend;
        return *this;
    }

    Bignum &operator-=(long long subtrahend)
    {
        *this = *this - Bignum(subtrahend);
        return *this;
    }

    // =========================================================================
    // Increment and Decrement Operators
    // =========================================================================

    /// Prefix increment (++a)
    Bignum &operator++()
    {
        *this += 1;
        return *this;
    }

    /// Postfix increment (a++)
    Bignum operator++(int)
    {
        Bignum temp = *this;
        *this += 1;
        return temp;
    }

    /// Prefix decrement (--a)
    Bignum &operator--()
    {
        *this -= 1;
        return *this;
    }

    /// Postfix decrement (a--)
    Bignum operator--(int)
    {
        Bignum temp = *this;
        *this -= 1;
        return temp;
    }

    // =========================================================================
    // Multiplication Operators
    // =========================================================================

    /**
     * @brief Multiplies two Bignum instances using schoolbook long multiplication.
     * @param multiplicand The value to multiply by.
     * @return The product.
     */
    Bignum operator*(const Bignum &multiplicand) const
    {
        if (isZero() || multiplicand.isZero())
        {
            return Bignum(0);
        }

        Bignum product;
        product.digits_.assign(digits_.size() + multiplicand.digits_.size(), 0);
        product.isNegative_ = (isNegative_ != multiplicand.isNegative_);

        for (size_t i = 0; i < digits_.size(); ++i)
        {
            long long carry = 0;
            for (size_t j = 0; j < multiplicand.digits_.size() || carry > 0; ++j)
            {
                long long current = product.digits_[i + j] + carry;
                if (j < multiplicand.digits_.size())
                {
                    current += static_cast<long long>(digits_[i]) * multiplicand.digits_[j];
                }
                product.digits_[i + j] = static_cast<int>(current % DEFAULT_BASE);
                carry = current / DEFAULT_BASE;
            }
        }

        product.normalize();
        return product;
    }

    Bignum operator*(long long multiplicand) const
    {
        return *this * Bignum(multiplicand);
    }

    Bignum &operator*=(const Bignum &multiplicand)
    {
        *this = *this * multiplicand;
        return *this;
    }

    Bignum &operator*=(long long multiplicand)
    {
        *this = *this * Bignum(multiplicand);
        return *this;
    }

    // =========================================================================
    // Division and Modulo Operators
    // =========================================================================

    /**
     * @brief Integer division using binary search quotient digit selection.
     * @param divisor The divisor Bignum.
     * @return Integer quotient.
     * @throws std::invalid_argument If divisor is zero.
     */
    Bignum operator/(const Bignum &divisor) const
    {
        if (divisor.isZero())
        {
            throw std::invalid_argument("Division by zero in Bignum::operator/");
        }

        if (compareAbsolute(*this, divisor) < 0)
        {
            return Bignum(0);
        }

        Bignum absoluteDividend = this->abs();
        Bignum absoluteDivisor = divisor.abs();

        Bignum currentRemainder = 0;
        std::vector<int> quotientDigits(digits_.size(), 0);

        for (int i = static_cast<int>(digits_.size()) - 1; i >= 0; --i)
        {
            currentRemainder.multiplyByTen();
            currentRemainder.digits_[0] = absoluteDividend.digits_[i];
            currentRemainder.normalize();

            // Binary search to find highest quotient digit in [0..9]
            int low = 0;
            int high = DEFAULT_BASE - 1;
            int bestDigit = 0;

            while (low <= high)
            {
                int mid = low + (high - low) / 2;
                Bignum testProduct = absoluteDivisor * mid;
                if (testProduct <= currentRemainder)
                {
                    bestDigit = mid;
                    low = mid + 1;
                }
                else
                {
                    high = mid - 1;
                }
            }

            quotientDigits[i] = bestDigit;
            if (bestDigit > 0)
            {
                currentRemainder -= (absoluteDivisor * bestDigit);
            }
        }

        Bignum quotient;
        quotient.digits_ = std::move(quotientDigits);
        quotient.isNegative_ = (isNegative_ != divisor.isNegative_);
        quotient.normalize();
        return quotient;
    }

    Bignum operator/(long long divisor) const
    {
        return *this / Bignum(divisor);
    }

    Bignum &operator/=(const Bignum &divisor)
    {
        *this = *this / divisor;
        return *this;
    }

    Bignum &operator/=(long long divisor)
    {
        *this = *this / Bignum(divisor);
        return *this;
    }

    /**
     * @brief Modulo (remainder) operation.
     * The sign of the remainder matches the sign of the dividend (standard C++ behavior).
     * @param modulo The modulus divisor.
     * @return Remainder after division.
     * @throws std::invalid_argument If modulo is zero.
     */
    Bignum operator%(const Bignum &modulo) const
    {
        if (modulo.isZero())
        {
            throw std::invalid_argument("Division by zero in Bignum::operator%");
        }

        if (compareAbsolute(*this, modulo) < 0)
        {
            return *this;
        }

        Bignum quotient = *this / modulo;
        Bignum remainder = *this - (quotient * modulo);
        return remainder;
    }

    Bignum operator%(long long modulo) const
    {
        return *this % Bignum(modulo);
    }

    Bignum &operator%=(const Bignum &modulo)
    {
        *this = *this % modulo;
        return *this;
    }

    Bignum &operator%=(long long modulo)
    {
        *this = *this % Bignum(modulo);
        return *this;
    }

    // =========================================================================
    // Mathematical Utilities
    // =========================================================================

    /**
     * @brief Computes the absolute value of the Bignum.
     * @return The non-negative magnitude.
     */
    [[nodiscard]] Bignum abs() const
    {
        Bignum result = *this;
        result.isNegative_ = false;
        return result;
    }

    /**
     * @brief Computes (*this)^exponent using binary exponentiation (exponentiation by squaring).
     * @param exponent The non-negative power to raise to.
     * @return (*this)^exponent.
     */
    [[nodiscard]] Bignum power(uint64_t exponent) const
    {
        if (exponent == 0)
        {
            return Bignum(1);
        }
        if (isZero())
        {
            return Bignum(0);
        }

        Bignum base = *this;
        Bignum result = 1;
        uint64_t currentExponent = exponent;

        while (currentExponent > 0)
        {
            if (currentExponent & 1)
            {
                result *= base;
            }
            if (currentExponent > 1)
            {
                base *= base;
            }
            currentExponent >>= 1;
        }

        return result;
    }

    /**
     * @brief Operator overload for exponentiation (*this ^ exponent).
     * @param exponent Non-negative integer power.
     * @return Power result.
     */
    Bignum operator^(uint64_t exponent) const
    {
        return power(exponent);
    }

    /**
     * @brief Computes the integer square root (floor(sqrt(*this))) using digit-by-digit root extraction.
     * @return The largest integer r such that r * r <= *this.
     * @throws std::domain_error If the number is negative.
     */
    [[nodiscard]] Bignum squareRoot() const
    {
        if (isNegative_)
        {
            throw std::domain_error("Square root of negative number is undefined in real integers");
        }
        if (isZero())
        {
            return Bignum(0);
        }
        if (*this == 1)
        {
            return Bignum(1);
        }

        // Group digits into pairs from right to left
        std::vector<int> digitPairs;
        for (size_t i = 0; i < digits_.size(); i += 2)
        {
            int pairValue = digits_[i];
            if (i + 1 < digits_.size())
            {
                pairValue += digits_[i + 1] * DEFAULT_BASE;
            }
            digitPairs.push_back(pairValue);
        }

        Bignum currentRemainder = 0;
        Bignum root = 0;

        // Process digit pairs from most significant to least significant
        for (int i = static_cast<int>(digitPairs.size()) - 1; i >= 0; --i)
        {
            // Shift remainder by two decimal digits (multiply by 100) and add the pair
            currentRemainder = (currentRemainder * 100) + digitPairs[i];

            // Find the greatest digit x in [0..9] such that (20 * root + x) * x <= currentRemainder
            int low = 0;
            int high = 9;
            int bestDigit = 0;
            Bignum rootTimesTwenty = root * 20;

            while (low <= high)
            {
                int mid = low + (high - low) / 2;
                Bignum test = (rootTimesTwenty + mid) * mid;
                if (test <= currentRemainder)
                {
                    bestDigit = mid;
                    low = mid + 1;
                }
                else
                {
                    high = mid - 1;
                }
            }

            currentRemainder -= (rootTimesTwenty + bestDigit) * bestDigit;
            root = (root * 10) + bestDigit;
        }

        return root;
    }


private:
    std::vector<int> digits_;   ///< Digits stored in little-endian order (digits_[0] is least significant).
    bool isNegative_ = false;   ///< Indicates whether the number is negative. Always false for zero.

    /**
     * @brief Internal helper to parse string input into digits and sign.
     */
    void fromString(const std::string &valueString)
    {
        digits_.clear();
        isNegative_ = false;

        if (valueString.empty())
        {
            throw std::invalid_argument("Empty string provided to Bignum constructor");
        }

        size_t startIndex = 0;
        while (startIndex < valueString.size() && std::isspace(static_cast<unsigned char>(valueString[startIndex])))
        {
            startIndex++;
        }

        if (startIndex >= valueString.size())
        {
            throw std::invalid_argument("String contains only whitespace");
        }

        if (valueString[startIndex] == '-')
        {
            isNegative_ = true;
            startIndex++;
        }
        else if (valueString[startIndex] == '+')
        {
            startIndex++;
        }

        if (startIndex >= valueString.size())
        {
            throw std::invalid_argument("No digits found after sign character");
        }

        // Parse digits in reverse order (least significant at index 0)
        for (size_t i = valueString.size(); i > startIndex; --i)
        {
            char character = valueString[i - 1];
            if (!std::isdigit(static_cast<unsigned char>(character)))
            {
                throw std::invalid_argument(std::string("Non-digit character encountered: '") + character + "'");
            }
            digits_.push_back(character - '0');
        }

        normalize();
    }

    /**
     * @brief Normalizes digit representation by propagating carries and removing leading zeros.
     */
    void normalize()
    {
        // Propagate carries
        for (size_t i = 0; i < digits_.size(); ++i)
        {
            if (digits_[i] >= DEFAULT_BASE)
            {
                int carry = digits_[i] / DEFAULT_BASE;
                digits_[i] %= DEFAULT_BASE;
                if (i + 1 < digits_.size())
                {
                    digits_[i + 1] += carry;
                }
                else
                {
                    digits_.push_back(carry);
                }
            }
        }

        // Remove trailing zeroes from little-endian vector (i.e. leading zeros of the number)
        while (digits_.size() > 1 && digits_.back() == 0)
        {
            digits_.pop_back();
        }

        if (digits_.empty())
        {
            digits_.push_back(0);
        }

        // If the number is zero, enforce non-negative sign
        if (digits_.size() == 1 && digits_[0] == 0)
        {
            isNegative_ = false;
        }
    }

    /**
     * @brief Compares absolute values of two Bignum instances.
     * @return -1 if |first| < |second|, 0 if |first| == |second|, 1 if |first| > |second|.
     */
    static int compareAbsolute(const Bignum &first, const Bignum &second) noexcept
    {
        if (first.digits_.size() < second.digits_.size())
        {
            return -1;
        }
        if (first.digits_.size() > second.digits_.size())
        {
            return 1;
        }
        for (int i = static_cast<int>(first.digits_.size()) - 1; i >= 0; --i)
        {
            if (first.digits_[i] < second.digits_[i])
            {
                return -1;
            }
            if (first.digits_[i] > second.digits_[i])
            {
                return 1;
            }
        }
        return 0;
    }

    /**
     * @brief Adds the absolute values of two Bignum instances (|first| + |second|).
     */
    static Bignum addAbsolute(const Bignum &first, const Bignum &second)
    {
        Bignum result;
        result.digits_.clear();
        size_t maximumLength = std::max(first.digits_.size(), second.digits_.size());
        int carry = 0;

        for (size_t i = 0; i < maximumLength || carry > 0; ++i)
        {
            int sum = carry;
            if (i < first.digits_.size())
            {
                sum += first.digits_[i];
            }
            if (i < second.digits_.size())
            {
                sum += second.digits_[i];
            }
            result.digits_.push_back(sum % DEFAULT_BASE);
            carry = sum / DEFAULT_BASE;
        }

        if (result.digits_.empty())
        {
            result.digits_.push_back(0);
        }
        return result;
    }

    /**
     * @brief Subtracts the absolute value of smaller from larger (|larger| - |smaller|).
     * Assumes |larger| >= |smaller|.
     */
    static Bignum subtractAbsolute(const Bignum &larger, const Bignum &smaller)
    {
        Bignum result;
        result.digits_.clear();
        int borrow = 0;

        for (size_t i = 0; i < larger.digits_.size(); ++i)
        {
            int difference = larger.digits_[i] - borrow - (i < smaller.digits_.size() ? smaller.digits_[i] : 0);
            if (difference < 0)
            {
                difference += DEFAULT_BASE;
                borrow = 1;
            }
            else
            {
                borrow = 0;
            }
            result.digits_.push_back(difference);
        }

        result.normalize();
        return result;
    }

    /**
     * @brief Fast in-place base-10 multiplication by shifting digits left.
     */
    void multiplyByTen()
    {
        if (isZero())
        {
            return;
        }
        digits_.insert(digits_.begin(), 0);
    }
};

// =============================================================================
// Non-member Stream and Arithmetic Overloads
// =============================================================================

inline std::ostream &operator<<(std::ostream &stream, const Bignum &number)
{
    stream << number.toString();
    return stream;
}

inline std::istream &operator>>(std::istream &stream, Bignum &number)
{
    std::string token;
    if (stream >> token)
    {
        number = Bignum(token);
    }
    return stream;
}

inline Bignum operator+(long long addend, const Bignum &number)
{
    return number + addend;
}

inline Bignum operator-(long long minuend, const Bignum &number)
{
    return Bignum(minuend) - number;
}

inline Bignum operator*(long long multiplier, const Bignum &number)
{
    return number * multiplier;
}

inline Bignum operator/(long long dividend, const Bignum &number)
{
    return Bignum(dividend) / number;
}

inline Bignum operator%(long long dividend, const Bignum &number)
{
    return Bignum(dividend) % number;
}

} // namespace bignum

// Expose Bignum globally for convenience
using bignum::Bignum;

#endif // BIGNUM_BIGNUM_HPP
