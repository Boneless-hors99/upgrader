#include "Currency.hpp"
#include <cmath>
#include <cstdint>

using namespace std;

BigNumber::BigNumber(float n, uint64_t e) : n(n), e(e) { norm(); }

void BigNumber::norm() {
  if (n == 0.0f) {
    e = 0.0f;
    return;
  }
  while (fabs(n) > 10.0f) {
    n /= 10.0f;
    e++;
  }
  while (fabs(n) < 1.0f) {
    n *= 10.0f;
    e--;
  }
}

bool operator==(const BigNumber &b1, const BigNumber &b2) {
  return (b1.n == b2.n) && (b1.e == b2.e);
}

bool operator>(const BigNumber &b1, const BigNumber &b2) {
  return (b1.e > b2.e) || (b2 > b1) || (b1.n > b2.n);
}

bool BigNumber::operator>=(const BigNumber &b) {
  return (*this == b) || (*this > b);
}

bool BigNumber::operator<(const BigNumber &b) { return !(*this >= b); }

bool BigNumber::operator<=(const BigNumber &b) { return !(*this > b); }
