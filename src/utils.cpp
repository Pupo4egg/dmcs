#include "utils.h"
#include "constants.h"
#include <iostream>

std::map<char, int> SYMBOLS_TO_INDEX;

void init_symbols_map() {
    for (int i = 0; i < MOD; ++i) {
        SYMBOLS_TO_INDEX[SYMBOLS[i]] = i;
    }
}

bool is_valid_digit(char c) {
    return SYMBOLS_TO_INDEX.count(c);
}

int get_symbol_weight(char c) {
    if (!is_valid_digit(c)) {
        throw std::logic_error("Internal error: get_symbol_weight called with invalid character.");
    }
    return SYMBOLS_TO_INDEX.at(c);
}

std::string get_min_value_str() {
    // Символ 'd' в новом порядке (a=0..f=7) имеет вес 6.
    return "-" + std::string(MAX_DIGITS, SYMBOLS[6]);
}

std::string get_max_value_str() {
    // Максимальное значение - 'd' (символ с весом 6), 8 раз.
    return std::string(MAX_DIGITS, SYMBOLS[6]);
}