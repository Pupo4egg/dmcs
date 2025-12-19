#include "BigFiniteNumber.h"
#include "constants.h"
#include "utils.h"
#include "arithmetic.h"
#include <algorithm>
#include <stdexcept>
#include <iostream>


std::string BigFiniteNumber::normalize(std::string num_str) const {
    size_t first_digit = num_str.find_first_not_of(get_additive_unit());
    if (std::string::npos == first_digit) {
        return std::string(1, get_additive_unit());
    }
    return num_str.substr(first_digit);
}

std::string BigFiniteNumber::pad_left(std::string num_str, int target_len) const {
    if (num_str.length() >= target_len) return num_str;
    return std::string(target_len - num_str.length(), get_additive_unit()) + num_str;
}

void BigFiniteNumber::check_overflow() const {
    if (value.length() > MAX_DIGITS) {
        throw std::overflow_error("");
    }
}

BigFiniteNumber::BigFiniteNumber(std::string val, bool neg) : value(normalize(val)), is_negative(neg) {
    if (this->value == std::string(1, get_additive_unit())) is_negative = false;
    check_overflow();
}

int BigFiniteNumber::compare_abs(const BigFiniteNumber& other) const {
    std::string s1 = normalize(this->value);
    std::string s2 = normalize(other.value);
    
    if (s1.length() > s2.length()) return 1;
    if (s1.length() < s2.length()) return -1;
    
    for (size_t i = 0; i < s1.length(); ++i) {
        int cmp = compare_chars(s1[i], s2[i]);
        if (cmp != 0) return cmp;
    }
    
    return 0;
}

BigFiniteNumber::BigFiniteNumber() : value(std::string(1, get_additive_unit())), is_negative(false) {}

BigFiniteNumber::BigFiniteNumber(const std::string& num_str) {
    if (num_str.empty()) {
        value = std::string(1, get_additive_unit()); 
        is_negative = false; 
        return;
    }
    
    std::string temp_str = num_str;
    is_negative = false;
    
    if (temp_str[0] == '-') {
        is_negative = true;
        temp_str = temp_str.substr(1);
        if (temp_str.empty()) {
            value = std::string(1, get_additive_unit()); 
            is_negative = false; 
            return;
        }
    }
    
    for (char c : temp_str) {
        if (!is_valid_digit(c)) {
            std::cerr << "ERROR: Invalid character '" << c << "' in input '" << num_str
                      << "'. Defaulting to '" << get_additive_unit() << "'." << std::endl;
            value = std::string(1, get_additive_unit()); 
            is_negative = false; 
            return;
        }
    }
    
    value = normalize(temp_str);
    if (value == std::string(1, get_additive_unit())) is_negative = false;
    check_overflow();
}

BigFiniteNumber BigFiniteNumber::from_internal_string(const std::string& val, bool neg) {
    return BigFiniteNumber(val, neg);
}

std::ostream& operator<<(std::ostream& os, const BigFiniteNumber& num) {
    if (num.is_negative && num.value != std::string(1, get_additive_unit())) {
        os << "-";
    }
    os << num.value;
    return os;
}

bool BigFiniteNumber::operator==(const BigFiniteNumber& other) const {
    return (this->is_negative == other.is_negative) && (this->value == other.value);
}

bool BigFiniteNumber::operator!=(const BigFiniteNumber& other) const { 
    return !(*this == other); 
}

bool BigFiniteNumber::operator<(const BigFiniteNumber& other) const {
    if (this->is_negative && !other.is_negative) return true;
    if (!this->is_negative && other.is_negative) return false;
    
    int cmp_abs = compare_abs(other);
    if (!this->is_negative) return cmp_abs == -1;
    else return cmp_abs == 1;
}

bool BigFiniteNumber::operator>(const BigFiniteNumber& other) const { 
    return other < *this; 
}

bool BigFiniteNumber::operator<=(const BigFiniteNumber& other) const { 
    return !(*this > other); 
}

bool BigFiniteNumber::operator>=(const BigFiniteNumber& other) const { 
    return !(*this < other); 
}

BigFiniteNumber BigFiniteNumber::operator+(const BigFiniteNumber& other) const {
    if (this->is_negative == other.is_negative) {
        
        int max_len = std::max(this->value.length(), other.value.length());
        std::string s1 = pad_left(this->value, max_len);
        std::string s2 = pad_left(other.value, max_len);
        
        std::string result;
        char perenos = get_additive_unit();
        for (int i = max_len - 1; i >= 0; --i) {
            auto [sum, new_perenos] = ADDITION_TABLE[std::make_tuple(s1[i], s2[i], perenos)];
            result = std::string(1, sum) + result;
            perenos = new_perenos;
        }
        
        if (perenos != get_additive_unit()) {
            result = std::string(1, perenos) + result;
        }
        
        BigFiniteNumber res = BigFiniteNumber::from_internal_string(normalize(result), this->is_negative);
        res.check_overflow();
        return res;
    } else {
        if (this->is_negative) {
            BigFiniteNumber positive_this = BigFiniteNumber::from_internal_string(this->value, false);
            return other - positive_this;
        } else {
            BigFiniteNumber positive_other = BigFiniteNumber::from_internal_string(other.value, false);
            return *this - positive_other;
        }
    }
}

BigFiniteNumber BigFiniteNumber::operator-(const BigFiniteNumber& other) const {
    
    if (this->is_negative != other.is_negative) {
        BigFiniteNumber positive_other = BigFiniteNumber::from_internal_string(other.value, !other.is_negative);
        return *this + positive_other;
    }
    
    int cmp = compare_abs(other);
    
    if (cmp == 0) {
        return BigFiniteNumber();
    }
    
    bool result_is_negative = this->is_negative;
    const BigFiniteNumber* larger = this;
    const BigFiniteNumber* smaller = &other;
    
    if (cmp < 0) {
        larger = &other;
        smaller = this;
        result_is_negative = !result_is_negative;
    }
    
    int max_len = std::max(larger->value.length(), smaller->value.length());
    std::string larger_padded = pad_left(larger->value, max_len);
    std::string smaller_padded = pad_left(smaller->value, max_len);
    
    std::string result;
    char zaem = get_additive_unit();
    
    for (int i = max_len - 1; i >= 0; --i) {
        char larger_char = larger_padded[i];
        char smaller_char = smaller_padded[i];
        
        if (zaem != get_additive_unit()) {
            if (compare_chars(larger_char, zaem) >= 0) {
                larger_char = symbolic_add(larger_char, symbolic_negate(zaem));
                zaem = get_additive_unit();
            } else {
                char temp = larger_char;
                for (int j = 0; j < MOD; j++) {
                    temp = next_symbol(temp);
                }
                larger_char = symbolic_add(temp, symbolic_negate(zaem));
                zaem = get_multiplicative_unit();
            }
        }
        
        if (compare_chars(larger_char, smaller_char) >= 0) {
            char diff = symbolic_add(larger_char, symbolic_negate(smaller_char));
            result = std::string(1, diff) + result;
        } else {
            char temp = larger_char;
            for (int j = 0; j < MOD; j++) {
                temp = next_symbol(temp);
            }
            char diff = symbolic_add(temp, symbolic_negate(smaller_char));
            result = std::string(1, diff) + result;
            zaem = get_multiplicative_unit();
        }
    }
    
    result = normalize(result);
    
    if (result == std::string(1, get_additive_unit())) {
        result_is_negative = false;
    }
    
    BigFiniteNumber res = BigFiniteNumber::from_internal_string(result, result_is_negative);
    res.check_overflow();
    return res;
}

BigFiniteNumber BigFiniteNumber::operator*(const BigFiniteNumber& other) const {
    if (this->value == std::string(1, get_additive_unit()) || other.value == std::string(1, get_additive_unit())) {
        return BigFiniteNumber::from_internal_string(std::string(1, get_additive_unit()), false);
    }

    bool result_negative = (this->is_negative != other.is_negative);
    
    BigFiniteNumber result = BigFiniteNumber::from_internal_string(std::string(1, get_additive_unit()), false);
    BigFiniteNumber multiplicand = BigFiniteNumber::from_internal_string(this->value, false);
    
    for (int i = other.value.length() - 1; i >= 0; i--) {
        char multiplier_digit = other.value[i];
        
        if (multiplier_digit != get_additive_unit()) {
            BigFiniteNumber partial_product = BigFiniteNumber::from_internal_string(std::string(1, get_additive_unit()), false);
            char counter = get_additive_unit();
            while (counter != multiplier_digit) {
                partial_product = partial_product + multiplicand;
                counter = next_symbol(counter);
            }
            
            int sdvig = other.value.length() - 1 - i;
            std::string sdvig_value = partial_product.value;
            for (int j = 0; j < sdvig; j++) {
                sdvig_value += get_additive_unit();
            }
            
            BigFiniteNumber sdvig_product = BigFiniteNumber::from_internal_string(sdvig_value, false);
            result = result + sdvig_product;
        }
    }
    
    BigFiniteNumber res = BigFiniteNumber::from_internal_string(result.value, result_negative);
    res.check_overflow();
    return res;
}

std::pair<BigFiniteNumber, BigFiniteNumber> BigFiniteNumber::divide(const BigFiniteNumber& other) const {
    if (this->value == std::string(1, get_additive_unit())) {
        BigFiniteNumber zero = BigFiniteNumber::from_internal_string(std::string(1, get_additive_unit()), false);
        return {zero, zero};
    }

    BigFiniteNumber dividend_abs = BigFiniteNumber::from_internal_string(this->value, false);
    BigFiniteNumber divisor_abs = BigFiniteNumber::from_internal_string(other.value, false);

    if (dividend_abs < divisor_abs) {
        if (this->is_negative) {
            BigFiniteNumber minus_one = BigFiniteNumber::from_internal_string(std::string(1, get_multiplicative_unit()), true);
            BigFiniteNumber remainder = divisor_abs - dividend_abs;
            return {minus_one, remainder};
        } else {
            BigFiniteNumber zero = BigFiniteNumber::from_internal_string(std::string(1, get_additive_unit()), false);
            return {zero, *this};
        }
    }

    std::string quotient_str;
    BigFiniteNumber current_remainder = BigFiniteNumber::from_internal_string(std::string(1, get_additive_unit()), false);
    
    for (size_t i = 0; i < dividend_abs.value.length(); i++) {
        current_remainder = BigFiniteNumber::from_internal_string(
            current_remainder.value + std::string(1, dividend_abs.value[i]), false
        );
        current_remainder.value = normalize(current_remainder.value);
        
        char quotient_digit = get_additive_unit();
        for (char test_digit = get_multiplicative_unit(); test_digit != get_additive_unit(); test_digit = next_symbol(test_digit)) {
            BigFiniteNumber test_product = divisor_abs * BigFiniteNumber::from_internal_string(std::string(1, test_digit), false);
            if (test_product > current_remainder) break;
            quotient_digit = test_digit;
        }
        quotient_str += quotient_digit;
        if (quotient_digit != get_additive_unit()) {
            BigFiniteNumber product = divisor_abs * BigFiniteNumber::from_internal_string(std::string(1, quotient_digit), false);
            current_remainder = current_remainder - product;
        }
    }
    
    BigFiniteNumber quotient_abs = BigFiniteNumber::from_internal_string(normalize(quotient_str), false);
    
    bool quotient_negative = (this->is_negative != other.is_negative);
    BigFiniteNumber quotient = BigFiniteNumber::from_internal_string(quotient_abs.value, quotient_negative);
    BigFiniteNumber remainder = current_remainder;
    
    if (this->is_negative && remainder.value != std::string(1, get_additive_unit())) {
        BigFiniteNumber one = BigFiniteNumber::from_internal_string(std::string(1, get_multiplicative_unit()), false);
        quotient = quotient - one;
        remainder = divisor_abs - remainder;
    }
    
    remainder.is_negative = false;
    
    return {quotient, remainder};
}