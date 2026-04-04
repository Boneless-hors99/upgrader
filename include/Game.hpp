#pragma once

#include "Upgrades.hpp"
#include <imgui_impl_glfw.h>
#include <unordered_map>

#define WINDOW_NAME "upgrader"
const ImVec2 DEFAULT_WINDOW_SIZE(1920.0f, 1080.0f);

struct GameState {
  std::unordered_map<Currencies, BigNumber> currencies;
};

class Game {
public:
  Game() : m_pos(0) {}
  void init();
  void loop();
  void end();

private:
  GameState m_state;

  GLFWwindow *m_window;
  ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);

  bool m_end = false;

  UpgradeVec m_pos;
  ImVec2 m_offset;

  void InitUpgrades();
  void InitWindow();

  void DrawUpgrades();
  void DrawAll();

  ImVec2 WindowSize();
};
