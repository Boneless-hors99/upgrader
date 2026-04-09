#pragma once

#include "Text.hpp"
#include "imgui.h"

enum BigNumberMode { SCIENTIFIC };

struct GameTheme {
  TextColor main;
  TextColor secondary;

  ImVec2 upgrade_padding;
  float upgrade_rounding;
  float connection_thickness;
  bool upgrade_random;

  float desc_rounding;
  float text_rounding;
  ImVec2 desc_padding;
  ImVec2 text_padding;

  float font_size;
};

struct Settings {
  static Settings &instance() {
    static Settings s;
    return s;
  }

  GameTheme theme;
  void SetTheme(GameTheme theme);
  void ApplyTheme();

  void SetMainTheme();

  ImFont *font;
  void LoadFont();

  BigNumberMode BNMode;
};
