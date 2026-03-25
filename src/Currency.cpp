#include "Currency.hpp"
#include <cmath>
#include <cstdint>

using namespace std;

BigNumber::BigNumber(float n, uint64_t e) : n(n), e(e) { norm(); }

BigNumber::BigNumber(float n) : BigNumber(n, 0) {}

void BigNumber::norm() {
  if (n == 0.0f) {
    e = 0;
    return;
  }
  while (fabs(n) > 10.0f) {
    n /= 10.0f;
    e++;
  }
  while (fabs(n) < 1.0f) {
    if (e == 0)
      return;
    n *= 10.0f;
    e--;
  }
}

float BigNumber::shiftTo(uint64_t new_e) const {
  float new_n = n;
  uint64_t temp_e = e;
  while (temp_e < new_e) {
    new_n /= 10;
    temp_e++;
  }
  while (temp_e > new_e) {
    new_n *= 10;
    temp_e--;
  }
  return new_n;
}

bool operator==(const BigNumber &b1, const BigNumber &b2) {
  return (b1.n == b2.n) && (b1.e == b2.e);
}

bool operator>(const BigNumber &b1, const BigNumber &b2) {
  if (b1.e != b2.e)
    return b1.e > b2.e;
  return b1.n > b2.n;
}

bool BigNumber::operator>=(const BigNumber &b) {
  return (*this == b) || (*this > b);
}

bool BigNumber::operator<(const BigNumber &b) { return !(*this >= b); }

bool BigNumber::operator<=(const BigNumber &b) { return !(*this > b); }

BigNumber BigNumber::operator+(const BigNumber &b) {
  if (e < b.e)
    return BigNumber(b.n + shiftTo(b.e), b.e);

  return BigNumber(n + b.shiftTo(e), e);
}

BigNumber BigNumber::operator-(const BigNumber &b) {
  if (b > *this)
    return BigNumber(0.0f);
  return BigNumber(n - b.shiftTo(e), e);
}
