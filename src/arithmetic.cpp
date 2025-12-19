#include "arithmetic.h"
#include "utils.h"
#include "constants.h"

char next_symbol(char current_char) {
    if (!is_valid_digit(current_char)) {
        throw std::logic_error("Internal error: next_symbol called with invalid character.");
    }
    return NEXT_SYMBOL_MAP.at(current_char);
}

char prev_symbol(char current_char) {
    if (!is_valid_digit(current_char)) {
        throw std::logic_error("Internal error: prev_symbol called with invalid character.");
    }
    char current = get_additive_unit();
    char prev = get_additive_unit();
    
    while (current != current_char) {
        prev = current;
        current = next_symbol(current);
    }
    
    return prev;
}

char symbolic_add(char char1, char char2) {
    if (!is_valid_digit(char1) || !is_valid_digit(char2)) {
        throw std::logic_error("Internal error: symbolic_add called with invalid characters.");
    }

    auto [sum, carry] = ADDITION_TABLE[std::make_tuple(char1, char2, get_additive_unit())];
    return sum;
}

char symbolic_negate(char c) {
    if (!is_valid_digit(c)) {
        throw std::logic_error("Internal error: symbolic_negate called with invalid character.");
    }
    return SYMBOL_NEGATION_MAP.at(c);
}

char symbolic_multiply(char char1, char char2) {
    if (!is_valid_digit(char1) || !is_valid_digit(char2)) {
        throw std::logic_error("Internal error: symbolic_multiply called with invalid characters.");
    }

    if (char1 == get_additive_unit() || char2 == get_additive_unit()) {
        return get_additive_unit();
    }
    
    if (char1 == get_multiplicative_unit()) return char2;
    if (char2 == get_multiplicative_unit()) return char1;
    
    char result = get_additive_unit();
    char counter = get_additive_unit();
    
    while (counter != char2) {
        result = symbolic_add(result, char1);
        counter = next_symbol(counter);
    }
    
    return result;
}