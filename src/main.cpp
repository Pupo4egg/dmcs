#include <iostream>
#include <string>
#include <algorithm>

#include "BigFiniteNumber.h"
#include "arithmetic.h"
#include "utils.h"
#include "constants.h"

// Предварительные объявления функций
BigFiniteNumber get_a_value();

// Отображает меню
void display_menu() {
    std::cout << "\n--- Большой Калькулятор Конечной Арифметики ---" << std::endl;
    std::cout << "Порядок символов (+1): " << SYMBOLS[0] << "(0) -> " << SYMBOLS[1] << "(1) -> " << SYMBOLS[2] << "(2) -> " << SYMBOLS[3] << "(3) -> " << SYMBOLS[4] << "(4) -> " << SYMBOLS[5] << "(5) -> " << SYMBOLS[6] << "(6) -> " << SYMBOLS[7] << "(7) -> " << SYMBOLS[0] << "(0)..." << std::endl;
    std::cout << "Аддитивная единица (0): " << ADDITIVE_UNIT << std::endl;
    std::cout << "Мультипликативная единица (1): " << MULTIPLICATIVE_UNIT << std::endl;
    std::cout << "8-разрядная арифметика." << std::endl;
    std::cout << "Диапазон: " << get_min_value_str() << " до " << get_max_value_str() << std::endl;
    std::cout << "Допустимые операции: +, -, *, /" << std::endl;
    std::cout << "Спец. случаи деления:" << std::endl;
    std::cout << "  " << ADDITIVE_UNIT << "/" << ADDITIVE_UNIT << " = " << INFINITY_INTERVAL_STR << std::endl;
    std::cout << "  " << ADDITIVE_UNIT << "/x (x!=" << ADDITIVE_UNIT << ") = " << ADDITIVE_UNIT << std::endl;
    std::cout << "  x/" << ADDITIVE_UNIT << " (x!=" << ADDITIVE_UNIT << ") = " << "пустое множество" << std::endl;
    std::cout << "Введите 'exit' для выхода." << std::endl;
    std::cout << "-----------------------------------------------" << std::endl;
}

// Проверяет, является ли строка корректным числом
bool is_valid_number_string(const std::string& s) {
    if (s.empty()) {
        std::cerr << "Ошибка: Пустая строка недопустима для числа." << std::endl;
        return false;
    }
    size_t start_idx = 0;
    if (s[0] == '-') {
        start_idx = 1;
        if (s.length() == 1) {
            std::cerr << "Ошибка: Введено только '-' как число. Это некорректно." << std::endl;
            return false;
        }
    }
    for (size_t i = start_idx; i < s.length(); ++i) {
        if (!is_valid_digit(s[i])) {
            std::cerr << "Ошибка: Недопустимый символ '" << s[i] << "' в числе '" << s << "'. Пожалуйста, используйте только символы из порядка: " << SYMBOLS[0] << " -> " << SYMBOLS[7] << "." << std::endl;
            return false;
        }
    }
    return true;
}

// Получить 'a' (ADDITIVE_UNIT) как BigFiniteNumber
BigFiniteNumber get_a_value() {
    return BigFiniteNumber::from_internal_string(std::string(1, ADDITIVE_UNIT), false);
}

int main() {
    init_symbols_maps();
    display_menu();

    std::string input_line;

    while (true) {
        std::cout << "\nВведите выражение (например, 'bcd + ef' или 'exit'): ";
        std::getline(std::cin, input_line);

        if (input_line == "exit") {
            break;
        }

        std::string s_num1, s_op, s_num2;
        size_t op_pos = std::string::npos;
        char found_op = '\0';

        // Поиск оператора, исключая '-' в начале числа
        for (size_t i = 0; i < input_line.length(); ++i) {
            char current_char = input_line[i];
            if (current_char == '+' || current_char == '*' || current_char == '/') {
                op_pos = i;
                found_op = current_char;
                break;
            } else if (current_char == '-' && i > 0) { // Оператор '-' НЕ на первой позиции
                op_pos = i;
                found_op = current_char;
                break;
            }
        }

        // Проверка на некорректный формат выражения
        if (found_op == '\0' || op_pos == std::string::npos) {
            if (input_line.length() == 1 && (input_line[0] == '+' || input_line[0] == '-' || input_line[0] == '*' || input_line[0] == '/')) {
                 std::cerr << "Ошибка: Введено только число. Ожидается 'число оператор число'." << std::endl;
            } else {
                std::cerr << "Ошибка: Некорректный формат выражения. Ожидается 'число оператор число'." << std::endl;
            }
            continue;
        }

        s_num1 = input_line.substr(0, op_pos);
        s_op = std::string(1, found_op);
        s_num2 = input_line.substr(op_pos + 1);

        auto trim = [](std::string& s) {
            s.erase(0, s.find_first_not_of(" \t"));
            s.erase(s.find_last_not_of(" \t") + 1);
        };
        trim(s_num1);
        trim(s_num2);

        if (s_num1.empty() || s_num2.empty()) {
            std::cerr << "Ошибка: Некорректный формат выражения. Недостаточно операндов." << std::endl;
            continue;
        }

        if (!is_valid_number_string(s_num1) || !is_valid_number_string(s_num2)) {
            continue;
        }

        try {
            BigFiniteNumber num1(s_num1);
            BigFiniteNumber num2(s_num2);
            BigFiniteNumber a_val = get_a_value(); // Получаем 'a'

            // --- Обработка особых случаев деления ---
            if (s_op == "/") {
                if (num1 == a_val && num2 == a_val) { // a / a
                    std::cout << "Результат: " << INFINITY_INTERVAL_STR << std::endl;
                    continue;
                } else if (num2 == a_val) { // x / a (где x != a)
                    std::cout << "Результат: пустое множество" << std::endl;
                    continue;
                } else if (num1 == a_val) { // a / x (где x != a)
                    std::cout << "Результат: " << a_val << std::endl;
                    continue;
                }
            }

            // --- Стандартные операции ---
            if (s_op == "+") {
                std::cout << "Результат: " << num1 + num2 << std::endl;
            } else if (s_op == "-") {
                std::cout << "Результат: " << num1 - num2 << std::endl;
            } else if (s_op == "*") {
                std::cout << "Результат: " << num1 * num2 << std::endl;
            } else if (s_op == "/") {
                auto [quotient, remainder] = num1.divide(num2);
                std::cout << "Частное: " << quotient << ", Остаток: " << remainder << std::endl;
            } else {
                 std::cerr << "Ошибка: Неизвестная операция '" << s_op << "'." << std::endl;
            }
        }
        catch (const std::runtime_error& e) {
            std::cerr << "Ошибка выполнения: " << e.what() << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "Непредвиденная ошибка: " << e.what() << std::endl;
        }
        catch (...) {
            std::cerr << "Произошла неизвестная критическая ошибка." << std::endl;
        }
    }

    std::cout << "Программа завершена." << std::endl;
    return 0;
}