#ifndef BIGFINITENUMBER_H
#define BIGFINITENUMBER_H

#include <iostream>
#include <string>
#include <utility>

class BigFiniteNumber {
private:
    std::string value;
    bool is_negative;

    std::string normalize(std::string num_str) const;
    std::string pad_left(std::string num_str, int target_len) const;
    void truncate();

    BigFiniteNumber(std::string val, bool neg);

public:
    BigFiniteNumber();
    BigFiniteNumber(const std::string& num_str);
    static BigFiniteNumber from_internal_string(const std::string& val, bool neg);

    int compare_abs(const BigFiniteNumber& other) const;

    friend std::ostream& operator<<(std::ostream& os, const BigFiniteNumber& num);

    bool operator==(const BigFiniteNumber& other) const;
    bool operator!=(const BigFiniteNumber& other) const;

    bool operator<(const BigFiniteNumber& other) const;
    bool operator>(const BigFiniteNumber& other) const;
    bool operator<=(const BigFiniteNumber& other) const;
    bool operator>=(const BigFiniteNumber& other) const;

    BigFiniteNumber operator+(const BigFiniteNumber& other) const;
    BigFiniteNumber operator-(const BigFiniteNumber& other) const;
    BigFiniteNumber operator*(const BigFiniteNumber& other) const;
    std::pair<BigFiniteNumber, BigFiniteNumber> divide(const BigFiniteNumber& other) const;
};

#endif