#pragma once

enum BigNumberMode { SCIENTIFIC };

struct Settings {
  static Settings operator()() {
    static Settings s;
    return s;
  }

  BigNumberMode BNMode;
};
