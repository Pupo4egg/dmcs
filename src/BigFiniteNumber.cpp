#include "BigFiniteNumber.h"
#include "constants.h"
#include "utils.h"
#include "arithmetic.h"
#include <algorithm> // Для std::reverse, std::max, std::swap
#include <stdexcept> // Для исключений
#include <iostream>

// Реализация методов класса BigFiniteNumber
std::string BigFiniteNumber::normalize(std::string num_str) const {
    size_t first_digit = num_str.find_first_not_of(ADDITIVE_UNIT);
    if (std::string::npos == first_digit) {
        return std::string(1, ADDITIVE_UNIT);
    }
    return num_str.substr(first_digit);
}

std::string BigFiniteNumber::pad_left(std::string num_str, int target_len) const {
    if (num_str.length() >= target_len) return num_str;
    return std::string(target_len - num_str.length(), ADDITIVE_UNIT) + num_str;
}

void BigFiniteNumber::truncate() {
    if (value.length() > MAX_DIGITS) {
        // === ДОБАВЛЕНО ПРЕДУПРЕЖДЕНИЕ О ПЕРЕПОЛНЕНИИ ===
        std::cerr << "WARNING: OVERFLOW detected! Result ("
                  << (is_negative ? "-" : "") << value
                  << ") exceeds " << MAX_DIGITS << " digits. Truncating." << std::endl;
        // === КОНЕЦ ДОБАВЛЕНИЯ ===

        if (!is_negative) { // Положительное число: отбрасываем старшие разряды
            value = value.substr(value.length() - MAX_DIGITS);
        }
        else { // Отрицательное число: берем последние MAX_DIGITS символов
            value = value.substr(value.length() - MAX_DIGITS);
        }
        value = normalize(value);
        if (value == std::string(1, ADDITIVE_UNIT)) is_negative = false; // 'a' не может быть отрицательным
    }
}

BigFiniteNumber::BigFiniteNumber(std::string val, bool neg) : value(normalize(val)), is_negative(neg) {
    if (this->value == std::string(1, ADDITIVE_UNIT)) is_negative = false;
    truncate(); // Обрезаем до 8 разрядов
}

int BigFiniteNumber::compare_abs(const BigFiniteNumber& other) const {
    std::string s1 = normalize(this->value);
    std::string s2 = normalize(other.value);
    if (s1.length() > s2.length()) return 1;
    if (s1.length() < s2.length()) return -1;
    for (size_t i = 0; i < s1.length(); ++i) {
        int w1 = get_symbol_weight(s1[i]);
        int w2 = get_symbol_weight(s2[i]);
        if (w1 > w2) return 1;
        if (w1 < w2) return -1;
    }
    return 0;
}

BigFiniteNumber::BigFiniteNumber() : value(std::string(1, ADDITIVE_UNIT)), is_negative(false) {}

BigFiniteNumber::BigFiniteNumber(const std::string& num_str) {
    if (num_str.empty()) {
        value = std::string(1, ADDITIVE_UNIT); is_negative = false; return;
    }
    std::string temp_str = num_str;
    is_negative = false;
    if (temp_str[0] == '-') {
        is_negative = true;
        temp_str = temp_str.substr(1);
        if (temp_str.empty()) {
             value = std::string(1, ADDITIVE_UNIT); is_negative = false; return;
        }
    }
    for (char c : temp_str) {
        if (!is_valid_digit(c)) {
            std::cerr << "ERROR: Invalid character '" << c << "' in input '" << num_str
                      << "'. Defaulting to 'a'." << std::endl;
            value = std::string(1, ADDITIVE_UNIT); is_negative = false; return;
        }
    }
    value = normalize(temp_str);
    if (value == std::string(1, ADDITIVE_UNIT)) is_negative = false;
    truncate(); // Обрезаем до 8 разрядов
}

BigFiniteNumber BigFiniteNumber::from_internal_string(const std::string& val, bool neg) {
    return BigFiniteNumber(val, neg); // Вызов приватного конструктора
}

std::ostream& operator<<(std::ostream& os, const BigFiniteNumber& num) {
    if (num.is_negative && num.value != std::string(1, ADDITIVE_UNIT)) {
        os << "-";
    }
    os << num.value;
    return os;
}

bool BigFiniteNumber::operator==(const BigFiniteNumber& other) const {
    return (this->is_negative == other.is_negative) && (this->value == other.value);
}
bool BigFiniteNumber::operator!=(const BigFiniteNumber& other) const { return !(*this == other); }

bool BigFiniteNumber::operator<(const BigFiniteNumber& other) const {
    if (this->is_negative && !other.is_negative) return true;
    if (!this->is_negative && other.is_negative) return false;
    int cmp_abs = compare_abs(other);
    if (!this->is_negative) return cmp_abs == -1;
    else return cmp_abs == 1;
}
bool BigFiniteNumber::operator>(const BigFiniteNumber& other) const { return other < *this; }
bool BigFiniteNumber::operator<=(const BigFiniteNumber& other) const { return !(*this > other); }
bool BigFiniteNumber::operator>=(const BigFiniteNumber& other) const { return !(*this < other); }

BigFiniteNumber BigFiniteNumber::operator+(const BigFiniteNumber& other) const {
    std::string s1 = this->value;
    std::string s2 = other.value;
    bool res_is_negative = this->is_negative;

    if (this->is_negative == other.is_negative) { // Знаки одинаковые
        int max_len = std::max(s1.length(), s2.length());
        s1 = pad_left(s1, max_len);
        s2 = pad_left(s2, max_len);

        std::string res_str = "";
        char carry = ADDITIVE_UNIT; // 'a'

        for (int i = max_len - 1; i >= 0; --i) {
            char sum_digits = symbolic_add(s1[i], s2[i]);
            char final_sum_char = symbolic_add(sum_digits, carry);
            int idx1 = get_symbol_weight(s1[i]);
            int idx2 = get_symbol_weight(s2[i]);
            int idx_carry = get_symbol_weight(carry);
            int total_index_sum = idx1 + idx2 + idx_carry;
            carry = SYMBOLS[total_index_sum / MOD];
            res_str += final_sum_char;
        }
        if (carry != ADDITIVE_UNIT) {
            res_str += carry;
        }
        std::reverse(res_str.begin(), res_str.end());
        return BigFiniteNumber::from_internal_string(res_str, res_is_negative);
    }
    else { // Знаки разные
        if (compare_abs(other) >= 0) {
            BigFiniteNumber temp_other = other;
            temp_other.is_negative = !temp_other.is_negative;
            return *this - temp_other;
        }
        else {
            BigFiniteNumber temp_this = *this;
            temp_this.is_negative = !temp_this.is_negative;
            return other - temp_this;
        }
    }
}

BigFiniteNumber BigFiniteNumber::operator-(const BigFiniteNumber& other) const {
    if (this->is_negative != other.is_negative) { // Знаки разные
        BigFiniteNumber temp_other = other;
        temp_other.is_negative = !temp_other.is_negative;
        return *this + temp_other;
    }
    else { // Знаки одинаковые
        std::string s1_abs = this->value;
        std::string s2_abs = other.value;
        bool res_is_negative = false;

        int cmp_abs = compare_abs(other);
        if (cmp_abs == 0) return BigFiniteNumber::from_internal_string(std::string(1, ADDITIVE_UNIT), false);

        if (cmp_abs < 0) {
            std::swap(s1_abs, s2_abs);
            res_is_negative = !this->is_negative;
        } else {
            res_is_negative = this->is_negative;
        }

        int max_len = std::max(s1_abs.length(), s2_abs.length());
        s1_abs = pad_left(s1_abs, max_len);
        s2_abs = pad_left(s2_abs, max_len);

        std::string res_str = "";
        char borrow = ADDITIVE_UNIT; // 'a'

        for (int i = max_len - 1; i >= 0; --i) {
            int idx1 = get_symbol_weight(s1_abs[i]);
            int idx2 = get_symbol_weight(s2_abs[i]);
            int idx_borrow = get_symbol_weight(borrow);

            idx1 -= idx_borrow;
            if (idx1 < idx2) {
                borrow = SYMBOLS[1]; // 'b' (1)
                idx1 += MOD;
            } else {
                borrow = ADDITIVE_UNIT; // 'a' (0)
            }
            res_str += SYMBOLS[idx1 - idx2];
        }
        std::reverse(res_str.begin(), res_str.end());
        return BigFiniteNumber::from_internal_string(res_str, res_is_negative);
    }
}

BigFiniteNumber BigFiniteNumber::operator*(const BigFiniteNumber& other) const {
    if (this->value == std::string(1, ADDITIVE_UNIT) || other.value == std::string(1, ADDITIVE_UNIT)) {
        return BigFiniteNumber::from_internal_string(std::string(1, ADDITIVE_UNIT), false);
    }

    std::string s1_abs = this->value;
    std::string s2_abs = other.value;
    bool res_is_negative = (this->is_negative != other.is_negative);

    std::string result_sum = std::string(1, ADDITIVE_UNIT); // 'a'

    for (int i = s2_abs.length() - 1; i >= 0; --i) {
        char digit2 = s2_abs[i];
        std::string current_partial_product = std::string(s2_abs.length() - 1 - i, ADDITIVE_UNIT); // Сдвиг

        char carry = ADDITIVE_UNIT; // 'a'
        for (int j = s1_abs.length() - 1; j >= 0; --j) {
            char digit1 = s1_abs[j];
            int idx1 = get_symbol_weight(digit1);
            int idx2 = get_symbol_weight(digit2);
            int product_val = idx1 * idx2 + get_symbol_weight(carry);

            carry = SYMBOLS[product_val / MOD];
            current_partial_product += SYMBOLS[product_val % MOD];
        }
        if (carry != ADDITIVE_UNIT) {
            current_partial_product += carry;
        }
        std::reverse(current_partial_product.begin(), current_partial_product.end());

        result_sum = (BigFiniteNumber::from_internal_string(result_sum, false) + BigFiniteNumber::from_internal_string(current_partial_product, false)).value;
    }
    return BigFiniteNumber::from_internal_string(result_sum, res_is_negative);
}

std::pair<BigFiniteNumber, BigFiniteNumber> BigFiniteNumber::divide(const BigFiniteNumber& other) const {
    // Деление на ноль ('a') - бросаем исключение
    if (other.value == std::string(1, ADDITIVE_UNIT)) {
        throw std::runtime_error("Division by zero ('a').");
    }

    // Особый случай: если делимое равно 'a' (0), то результат 'a'
    if (this->value == std::string(1, ADDITIVE_UNIT)) {
        BigFiniteNumber zero = BigFiniteNumber::from_internal_string(std::string(1, ADDITIVE_UNIT), false);
        return {zero, zero};
    }

    BigFiniteNumber A_abs = BigFiniteNumber::from_internal_string(this->value, false);
    BigFiniteNumber B_abs = BigFiniteNumber::from_internal_string(other.value, false);

    std::string Q_abs_str = "";
    BigFiniteNumber current_remainder_abs = BigFiniteNumber::from_internal_string(std::string(1, ADDITIVE_UNIT), false); // 'a'

    if (A_abs < B_abs) {
        Q_abs_str = std::string(1, ADDITIVE_UNIT); // 'a'
        current_remainder_abs = A_abs;
    } else {
        for (char digit : A_abs.value) {
            current_remainder_abs.value += digit;
            current_remainder_abs.value = current_remainder_abs.normalize(current_remainder_abs.value);

            char q_digit_char = ADDITIVE_UNIT; // 'a'
            int q_digit_weight = 0;

            for (int i = 0; i < MOD; ++i) {
                BigFiniteNumber test_q_digit = BigFiniteNumber::from_internal_string(std::string(1, SYMBOLS[i]), false);
                BigFiniteNumber test_product = B_abs * test_q_digit;

                if (current_remainder_abs < test_product) {
                    q_digit_weight = i - 1;
                    if (q_digit_weight < 0) q_digit_weight = 0;
                    break;
                }
                q_digit_weight = i;
            }
            q_digit_char = SYMBOLS[q_digit_weight];
            Q_abs_str += q_digit_char;

            BigFiniteNumber q_digit_num = BigFiniteNumber::from_internal_string(std::string(1, q_digit_char), false);
            BigFiniteNumber subtrahend = B_abs * q_digit_num;
            current_remainder_abs = current_remainder_abs - subtrahend;
        }
    }

    BigFiniteNumber Q_abs = BigFiniteNumber::from_internal_string(Q_abs_str, false);
    BigFiniteNumber R_abs = current_remainder_abs;

    bool Q_is_negative = (this->is_negative != other.is_negative);
    BigFiniteNumber Q_initial = BigFiniteNumber::from_internal_string(Q_abs.value, Q_is_negative);

    BigFiniteNumber R_calc = *this - (other * Q_initial);

    BigFiniteNumber Q_final = Q_initial;
    BigFiniteNumber R_final = R_calc;
    BigFiniteNumber one = BigFiniteNumber::from_internal_string(std::string(1, MULTIPLICATIVE_UNIT), false); // 'b' (1)

    // Коррекция для положительного остатка 0 <= R < |B|
    if (R_final < BigFiniteNumber::from_internal_string(std::string(1, ADDITIVE_UNIT), false)) { // R_calc < 'a' (0)
        Q_final = Q_final - one;
        R_final = R_final + B_abs;
    }
    else if (R_final >= B_abs && R_final != BigFiniteNumber::from_internal_string(std::string(1, ADDITIVE_UNIT), false)) {
         Q_final = Q_final + one;
         R_final = R_final - B_abs;
    }

    return {Q_final, R_final};
}