#include "BigFiniteNumber.h"
#include "constants.h"
#include "utils.h"
#include "arithmetic.h"
#include <algorithm>
#include <stdexcept>
#include <iostream>

// Реализация методов класса BigFiniteNumber
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

void BigFiniteNumber::truncate() {
    if (value.length() > MAX_DIGITS) {
        std::cerr << "WARNING: OVERFLOW detected! Result ("
                  << (is_negative ? "-" : "") << value
                  << ") exceeds " << MAX_DIGITS << " digits. Truncating." << std::endl;

        value = value.substr(value.length() - MAX_DIGITS);
        value = normalize(value);
        if (value == std::string(1, get_additive_unit())) is_negative = false;
    }
}

BigFiniteNumber::BigFiniteNumber(std::string val, bool neg) : value(normalize(val)), is_negative(neg) {
    if (this->value == std::string(1, get_additive_unit())) is_negative = false;
    truncate();
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
    truncate();
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
        // Сложение чисел с одинаковыми знаками
        int max_len = std::max(this->value.length(), other.value.length());
        std::string s1 = pad_left(this->value, max_len);
        std::string s2 = pad_left(other.value, max_len);
        
        std::string result;
        char carry = get_additive_unit();
        
        for (int i = max_len - 1; i >= 0; --i) {
            auto [sum, new_carry] = ADDITION_TABLE[std::make_tuple(s1[i], s2[i], carry)];
            result = std::string(1, sum) + result;
            carry = new_carry;
        }
        
        // Если остался перенос, добавляем его
        if (carry != get_additive_unit()) {
            result = std::string(1, carry) + result;
        }
        
        return BigFiniteNumber::from_internal_string(normalize(result), this->is_negative);
    } else {
        // Разные знаки - сводим к вычитанию
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
        // Разные знаки - сводим к сложению
        BigFiniteNumber positive_other = BigFiniteNumber::from_internal_string(other.value, !other.is_negative);
        return *this + positive_other;
    } else {
        // Одинаковые знаки
        int cmp = compare_abs(other);
        if (cmp == 0) {
            return BigFiniteNumber::from_internal_string(std::string(1, get_additive_unit()), false);
        }
        
        bool result_negative = this->is_negative;
        if (cmp < 0) {
            // Если |this| < |other|, результат меняет знак
            result_negative = !result_negative;
        }
        
        std::string larger = (cmp >= 0) ? this->value : other.value;
        std::string smaller = (cmp >= 0) ? other.value : this->value;
        
        // Выравниваем по длине
        int max_len = std::max(larger.length(), smaller.length());
        larger = pad_left(larger, max_len);
        smaller = pad_left(smaller, max_len);
        
        std::string result;
        char borrow = get_additive_unit();
        
        for (int i = max_len - 1; i >= 0; --i) {
            char current = larger[i];
            
            // Вычитаем borrow
            if (borrow != get_additive_unit()) {
                current = symbolic_add(current, symbolic_negate(borrow));
                borrow = get_additive_unit();
            }
            
            // Если текущая цифра меньше вычитаемой, берем заем
            if (compare_chars(current, smaller[i]) < 0) {
                borrow = get_multiplicative_unit();
                // Добавляем MOD к текущей цифре
                char mod_value = get_additive_unit();
                for (int j = 0; j < MOD; j++) {
                    mod_value = next_symbol(mod_value);
                }
                current = symbolic_add(current, mod_value);
            }
            
            char result_digit = symbolic_add(current, symbolic_negate(smaller[i]));
            result = std::string(1, result_digit) + result;
        }
        
        return BigFiniteNumber::from_internal_string(normalize(result), result_negative);
    }
}

BigFiniteNumber BigFiniteNumber::operator*(const BigFiniteNumber& other) const {
    if (this->value == std::string(1, get_additive_unit()) || other.value == std::string(1, get_additive_unit())) {
        return BigFiniteNumber::from_internal_string(std::string(1, get_additive_unit()), false);
    }

    bool result_negative = (this->is_negative != other.is_negative);
    
    // Используем метод сложения со сдвигами
    BigFiniteNumber result = BigFiniteNumber::from_internal_string(std::string(1, get_additive_unit()), false);
    BigFiniteNumber multiplicand = BigFiniteNumber::from_internal_string(this->value, false);
    
    // Проходим по каждой цифре множителя справа налево
    for (int i = other.value.length() - 1; i >= 0; i--) {
        char multiplier_digit = other.value[i];
        
        if (multiplier_digit != get_additive_unit()) {
            // Умножаем multiplicand на текущую цифру множителя через сложение
            BigFiniteNumber partial_product = BigFiniteNumber::from_internal_string(std::string(1, get_additive_unit()), false);
            
            char counter = get_additive_unit();
            while (counter != multiplier_digit) {
                partial_product = partial_product + multiplicand;
                counter = next_symbol(counter);
            }
            
            // Сдвиг влево (добавляем нули справа)
            int shift_count = other.value.length() - 1 - i;
            std::string shifted_value = partial_product.value;
            for (int j = 0; j < shift_count; j++) {
                shifted_value += get_additive_unit();
            }
            
            BigFiniteNumber shifted_product = BigFiniteNumber::from_internal_string(shifted_value, false);
            result = result + shifted_product;
        }
    }
    
    return BigFiniteNumber::from_internal_string(result.value, result_negative);
}

std::pair<BigFiniteNumber, BigFiniteNumber> BigFiniteNumber::divide(const BigFiniteNumber& other) const {
    if (other.value == std::string(1, get_additive_unit())) {
        throw std::runtime_error("Division by zero ('a').");
    }

    if (this->value == std::string(1, get_additive_unit())) {
        BigFiniteNumber zero = BigFiniteNumber::from_internal_string(std::string(1, get_additive_unit()), false);
        return {zero, zero};
    }

    // Работаем с абсолютными значениями
    BigFiniteNumber dividend_abs = BigFiniteNumber::from_internal_string(this->value, false);
    BigFiniteNumber divisor_abs = BigFiniteNumber::from_internal_string(other.value, false);

    // Если делимое меньше делителя
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

    // Деление в столбик
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
    
    // Определяем знаки
    bool quotient_negative = (this->is_negative != other.is_negative);
    BigFiniteNumber quotient = BigFiniteNumber::from_internal_string(quotient_abs.value, quotient_negative);
    BigFiniteNumber remainder = current_remainder;
    
    // Корректировка для отрицательных
    if (this->is_negative && remainder.value != std::string(1, get_additive_unit())) {
        BigFiniteNumber one = BigFiniteNumber::from_internal_string(std::string(1, get_multiplicative_unit()), false);
        quotient = quotient - one;
        remainder = divisor_abs - remainder;
    }
    
    remainder.is_negative = false;
    
    return {quotient, remainder};
}