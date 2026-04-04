#pragma once

#include "Text.hpp"
#include <cstdint>
#include <string>

class BigNumber {
public:
  BigNumber(float n, uint64_t e);
  BigNumber(float n);
  void norm();
  float shiftTo(uint64_t new_e) const;

  friend bool operator==(const BigNumber &b1, const BigNumber &b2);
  friend bool operator>(const BigNumber &b1, const BigNumber &b2);
  bool operator>=(const BigNumber &b);
  bool operator<(const BigNumber &b);
  bool operator<=(const BigNumber &b);

  BigNumber operator+(const BigNumber &b);
  void operator+=(const BigNumber &b);

  BigNumber operator-(const BigNumber &b);
  void operator-=(const BigNumber &b);

  std::string toString();

private:
  float n;
  uint64_t e;
};

enum class Currencies { Currency_X };

std::string CtoString(Currencies currency);

struct Price {
  Price(Currencies cur, BigNumber amt) : cur(cur), amt(amt) {}
  Price(BigNumber amt) : cur(Currencies::Currency_X), amt(amt) {}

  Currencies cur;
  BigNumber amt;

  std::string toString();
  Text toText();
};
