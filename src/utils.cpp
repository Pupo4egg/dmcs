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
    
    NEXT_SYMBOL_MAP['a'] = 'b';
    NEXT_SYMBOL_MAP['b'] = 'e'; 
    NEXT_SYMBOL_MAP['e'] = 'c';
    NEXT_SYMBOL_MAP['c'] = 'g';
    NEXT_SYMBOL_MAP['g'] = 'h';
    NEXT_SYMBOL_MAP['h'] = 'd';
    NEXT_SYMBOL_MAP['d'] = 'f';
    NEXT_SYMBOL_MAP['f'] = 'a';  
    

    SYMBOL_NEGATION_MAP['a'] = 'a';
    SYMBOL_NEGATION_MAP['b'] = 'f';
    SYMBOL_NEGATION_MAP['e'] = 'd';
    SYMBOL_NEGATION_MAP['c'] = 'h';
    SYMBOL_NEGATION_MAP['g'] = 'g';  
    SYMBOL_NEGATION_MAP['h'] = 'c';
    SYMBOL_NEGATION_MAP['d'] = 'e';
    SYMBOL_NEGATION_MAP['f'] = 'b';
    
   
    for (char c1 : SYMBOLS) {
        for (char c2 : SYMBOLS) {
            for (char carry_in : SYMBOLS) {

                char sum1 = symbolic_add_simple(c1, c2);
                char final_sum = symbolic_add_simple(sum1, carry_in);
                

                char carry_out = get_additive_unit(); 
                
                //был ли перенос при первом сложении
                bool carry_from_first = (c1 != get_additive_unit() && c2 != get_additive_unit() && 
                                        compare_chars(sum1, c1) < 0 && compare_chars(sum1, c2) < 0);
                
                //был ли перенос при втором сложении
                bool carry_from_second = (sum1 != get_additive_unit() && carry_in != get_additive_unit() &&
                                         compare_chars(final_sum, sum1) < 0 && compare_chars(final_sum, carry_in) < 0);
                

               /* if (c1 != get_additive_unit() && c2 != get_additive_unit()) {

                    char current = c1;
                    bool passed_through_a = false;
                    
                    for (char cnt = get_additive_unit(); cnt != c2; cnt = NEXT_SYMBOL_MAP.at(cnt)) {
                        current = NEXT_SYMBOL_MAP.at(current);
                        if (current == get_additive_unit() && cnt != c2) {
                            passed_through_a = true;
                            break;
                        }
                    }
                    
                    if (passed_through_a) {
                        carry_from_first = true;
                    }
                }
                

                if (sum1 != get_additive_unit() && carry_in != get_additive_unit()) {
                    char current = sum1;
                    bool passed_through_a = false;
                    
                    for (char cnt = get_additive_unit(); cnt != carry_in; cnt = NEXT_SYMBOL_MAP.at(cnt)) {
                        current = NEXT_SYMBOL_MAP.at(current);
                        if (current == get_additive_unit() && cnt != carry_in) {
                            passed_through_a = true;
                            break;
                        }
                    }
                    
                    if (passed_through_a) {
                        carry_from_second = true;
                    }
                }*/
                
                if (carry_from_first || carry_from_second) {
                    carry_out = get_multiplicative_unit();
                }
                
                ADDITION_TABLE[std::make_tuple(c1, c2, carry_in)] = std::make_pair(final_sum, carry_out);
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
    
    char current = get_additive_unit();
    while (true) {
        if (current == c1) return -1;
        if (current == c2) return 1;
        current = NEXT_SYMBOL_MAP.at(current);
        if (current == get_additive_unit()) break; 
    }
    return 0; 
}