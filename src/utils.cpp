#include "utils.h"
#include "constants.h"
#include <iostream>

std::map<char, char> NEXT_SYMBOL_MAP;
std::map<char, char> SYMBOL_NEGATION_MAP;
std::map<std::tuple<char, char, char>, std::pair<char, char>> ADDITION_TABLE;

char symbolic_add_simple(char char1, char char2) {
    if (char1 == get_additive_unit()) return char2;
    if (char2 == get_additive_unit()) return char1;
    
    char result = char1;
    char counter = get_additive_unit();
    
    while (counter != char2) {
        result = NEXT_SYMBOL_MAP.at(result);
        counter = NEXT_SYMBOL_MAP.at(counter);
    }
    
    return result;
}

void init_symbols_maps() {
    // Инициализируем правило +1 через явное задание цикла
    NEXT_SYMBOL_MAP['a'] = 'b';
    NEXT_SYMBOL_MAP['b'] = 'e'; 
    NEXT_SYMBOL_MAP['e'] = 'c';
    NEXT_SYMBOL_MAP['c'] = 'g';
    NEXT_SYMBOL_MAP['g'] = 'h';
    NEXT_SYMBOL_MAP['h'] = 'd';
    NEXT_SYMBOL_MAP['d'] = 'f';
    NEXT_SYMBOL_MAP['f'] = 'a';  // Замыкаем цикл
    
    // Инициализируем таблицу отрицаний (x + neg(x) = 'a')
    SYMBOL_NEGATION_MAP['a'] = 'a';
    SYMBOL_NEGATION_MAP['b'] = 'f';
    SYMBOL_NEGATION_MAP['e'] = 'd';
    SYMBOL_NEGATION_MAP['c'] = 'h';
    SYMBOL_NEGATION_MAP['g'] = 'g';  // Сам себе обратный
    SYMBOL_NEGATION_MAP['h'] = 'c';
    SYMBOL_NEGATION_MAP['d'] = 'e';
    SYMBOL_NEGATION_MAP['f'] = 'b';
    
    // Инициализируем таблицу сложения с переносами
    for (char c1 : SYMBOLS) {
        for (char c2 : SYMBOLS) {
            for (char carry_in : SYMBOLS) {
                // Складываем три символа
                char sum = symbolic_add_simple(symbolic_add_simple(c1, c2), carry_in);
                
                // Определяем перенос
                char carry_out = get_additive_unit();
                
                // Эмулируем сложение в десятичной системе для определения переноса
                int total = 0;
                char current = get_additive_unit();
                
                // Считаем c1
                while (current != c1) {
                    total++;
                    current = NEXT_SYMBOL_MAP.at(current);
                }
                
                // Считаем c2
                current = get_additive_unit();
                while (current != c2) {
                    total++;
                    current = NEXT_SYMBOL_MAP.at(current);
                }
                
                // Считаем carry_in
                current = get_additive_unit();
                while (current != carry_in) {
                    total++;
                    current = NEXT_SYMBOL_MAP.at(current);
                }
                
                // Если сумма превышает MOD-1, есть перенос
                if (total >= MOD) {
                    carry_out = get_multiplicative_unit();
                }
                
                ADDITION_TABLE[std::make_tuple(c1, c2, carry_in)] = std::make_pair(sum, carry_out);
            }
        }
    }
}

bool is_valid_digit(char c) {
    return NEXT_SYMBOL_MAP.count(c);
}

char get_additive_unit() {
    return 'a';
}

char get_multiplicative_unit() {
    return 'b';
}

std::string get_min_value_str() {
    return "-" + std::string(MAX_DIGITS, 'f');
}

std::string get_max_value_str() {
    return std::string(MAX_DIGITS, 'f');
}

int compare_chars(char c1, char c2) {
    if (c1 == c2) return 0;
    
    // Проходим по циклу, чтобы определить порядок
    char current = get_additive_unit();
    while (true) {
        if (current == c1) return -1;
        if (current == c2) return 1;
        current = NEXT_SYMBOL_MAP.at(current);
        if (current == get_additive_unit()) break; // Прошли полный цикл
    }
    return 0; // Не должно случиться
}