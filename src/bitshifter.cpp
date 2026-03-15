#include "Upgrades.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
  int32_t x, y;
  x = std::atol(argv[1]);
  y = std::atol(argv[2]);
  UpgradeVec vec(x, y);
  std::cout << vec.i64() << std::endl;
  return 0;
}
