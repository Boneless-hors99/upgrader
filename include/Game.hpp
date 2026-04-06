#pragma once

#include "Currency.hpp"
#include "Upgrades.hpp"
#include <imgui_impl_glfw.h>
#include <thread>
#include <unordered_map>

#define WINDOW_NAME "upgrader"
const ImVec2 DEFAULT_WINDOW_SIZE(1920.0f, 1080.0f);

enum class Threads {
  CURRENCY_GAINS,
};

struct GameState {
  static GameState &instance() {
    static GameState gs;
    return gs;
  }
  bool running;
  std::unordered_map<Currencies, BigNumber> currencies;
  std::unordered_map<Currencies, CurrencyGain> currency_gains;
};

class Game {
public:
  Game() : m_pos(0) {}
  void init();
  void loop();
  void end();

private:
  GLFWwindow *m_window;
  ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);

  UpgradeVec m_pos;
  ImVec2 m_offset;

  std::unordered_map<Threads, std::thread> m_threads;

  void InitUpgrades();
  void InitWindow();
  void InitThreads();

  void Render();
  static void UpdateCurrencies(float delta);

#ifdef DEBUG
  void DrawDebug();
#endif // DEBUG

  void DrawUpgrades();
  void DrawAll();

  ImVec2 WindowSize();
};
