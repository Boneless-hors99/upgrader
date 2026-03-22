#include "Upgrades.hpp"
#include <imgui_impl_glfw.h>

#define WINDOW_NAME "upgrader"
const ImVec2 DEFAULT_WINDOW_SIZE(1920.0f, 1080.0f);

class Game {
public:
  Game() : m_pos(0) {}
  void init();
  void loop();
  void end();

private:
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
