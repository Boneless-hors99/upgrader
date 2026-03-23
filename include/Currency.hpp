#pragma once

#include <cstdint>

class BigNumber {
public:
  BigNumber(float n, uint64_t e);
  void norm();

  friend bool operator==(const BigNumber &b1, const BigNumber &b2);
  friend bool operator>(const BigNumber &b1, const BigNumber &b2);
  bool operator>=(const BigNumber &b);
  bool operator<(const BigNumber &b);
  bool operator<=(const BigNumber &b);

private:
  float n;
  uint64_t e;
};
