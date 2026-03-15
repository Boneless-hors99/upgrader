#include "Upgrades.hpp"
#include <raylib.h>

#define WINDOW_NAME "upgrader"
const Vector2 DEFAULT_WINDOW_SIZE(1920.0f, 1080.0f);

class Game {
public:
  Game() : m_pos(0) {}
  void init();
  void loop();
  void end();

private:
  bool m_end = false;

  UpgradeVec m_pos;
  Vector2 m_offset;

  void InitUpgrades();
  void InitWindow();

  void DrawUpgrades();
  void DrawAll();

  Vector2 WindowSize();
};
