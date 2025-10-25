#include "arithmetic.h"
#include "utils.h"
#include "constants.h"

char next_symbol(char current_char) {
    int index = get_symbol_weight(current_char);
    int next_index = (index + 1) % MOD;
    return SYMBOLS[next_index];
}

char symbolic_add(char char1, char char2) {
    if (!is_valid_digit(char1) || !is_valid_digit(char2)) {
        throw std::logic_error("Internal error: symbolic_add called with invalid characters.");
    }
    char result = char1;
    int y_weight = get_symbol_weight(char2);
    for (int i = 0; i < y_weight; ++i) {
        result = next_symbol(result);
    }
    return result;
}

char symbolic_negate(char c) {
    if (!is_valid_digit(c)) {
        throw std::logic_error("Internal error: symbolic_negate called with invalid character.");
    }
    if (c == ADDITIVE_UNIT) return ADDITIVE_UNIT;
    for (char potential_neg : SYMBOLS) {
        if (symbolic_add(c, potential_neg) == ADDITIVE_UNIT) {
            return potential_neg;
        }
    }
    throw std::logic_error("Internal error: symbolic_negate failed to find negation.");
}

char symbolic_multiply(char char1, char char2) {
    if (!is_valid_digit(char1) || !is_valid_digit(char2)) {
        throw std::logic_error("Internal error: symbolic_multiply called with invalid characters.");
    }
    if (char1 == ADDITIVE_UNIT || char2 == ADDITIVE_UNIT) return ADDITIVE_UNIT;
    if (char1 == MULTIPLICATIVE_UNIT) return char2;
    if (char2 == MULTIPLICATIVE_UNIT) return char1;
    char result = ADDITIVE_UNIT;
    int y_weight = get_symbol_weight(char2);
    for (int i = 0; i < y_weight; ++i) {
        result = symbolic_add(result, char1);
    }
    return result;
}