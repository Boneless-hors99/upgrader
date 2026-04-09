#pragma once

#include "Text.hpp"
#include <cstdint>
#include <string>

class BigNumber {
public:
  BigNumber() : n(0), e(0) {}
  BigNumber(float n, int64_t e);
  BigNumber(float n);
  void norm();
  float shiftTo(int64_t new_e) const;

  friend bool operator==(const BigNumber &b1, const BigNumber &b2);
  friend bool operator>(const BigNumber &b1, const BigNumber &b2);
  bool operator>=(const BigNumber &b) const;
  bool operator<(const BigNumber &b) const;
  bool operator<=(const BigNumber &b) const;

  BigNumber operator+(const BigNumber &b) const;
  void operator+=(const BigNumber &b);

  BigNumber operator-(const BigNumber &b) const;
  void operator-=(const BigNumber &b);

  BigNumber operator*(const BigNumber &b) const;
  void operator*=(const BigNumber &b);

  BigNumber operator*(const float &f) const;

  std::string toString();
  void Selector();

private:
  float n;
  uint64_t e;
};

enum class Currencies { Currency_X };

std::string CtoString(Currencies currency);

struct Currency {
  Currency(Currencies cur, BigNumber amt) : cur(cur), amt(amt) {}
  Currency(BigNumber amt) : cur(Currencies::Currency_X), amt(amt) {}
  Currency(float amt) : cur(Currencies::Currency_X), amt(amt) {}

  Currencies cur;
  BigNumber amt;

  virtual std::string toString();
  Text toText();
};

struct Price : public Currency {
  using Currency::Currency;

  std::string toString() override;
};

struct CurrencyGain {
  CurrencyGain() : base(0), mult(1), exp(1) {}
  CurrencyGain(BigNumber base, BigNumber mult, BigNumber exp)
      : base(base), mult(mult), exp(exp) {}
  BigNumber base;
  BigNumber mult;
  BigNumber exp;

  BigNumber get() const;
  BigNumber operator()() const;

  void operator+=(const BigNumber &b);
  void operator*=(const BigNumber &b);
  void operator^=(const BigNumber &b);

  std::string toString(bool parenthesis = false);
};
