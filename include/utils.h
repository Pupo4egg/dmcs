#ifndef UTILS_H
#define UTILS_H

#include <map>
#include <stdexcept>
#include <string>

extern std::map<char, int> SYMBOLS_TO_INDEX;

void init_symbols_map();
bool is_valid_digit(char c);
int get_symbol_weight(char c);
std::string get_min_value_str();
std::string get_max_value_str();

#endif