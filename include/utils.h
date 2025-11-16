#ifndef UTILS_H
#define UTILS_H

#include <map>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>

extern std::map<char, char> NEXT_SYMBOL_MAP;
extern std::map<char, char> SYMBOL_NEGATION_MAP;
extern std::map<std::tuple<char, char, char>, std::pair<char, char>> ADDITION_TABLE;

void init_symbols_maps();
bool is_valid_digit(char c);
char get_additive_unit();
char get_multiplicative_unit();
std::string get_min_value_str();
std::string get_max_value_str();
int compare_chars(char c1, char c2);

#endif