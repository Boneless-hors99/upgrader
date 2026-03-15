#include "Game.hpp"
#include "Upgrades.hpp"
#include <raylib.h>

void Game::InitUpgrades() {
  m_pos = UpgradeVec(0);
  m_offset = Vector2(0.0f);
  auto &upgrades = UpgradeManager::instance();
  upgrades.init();
}

void Game::InitWindow() {
  ::InitWindow(DEFAULT_WINDOW_SIZE.x, DEFAULT_WINDOW_SIZE.y, WINDOW_NAME);
}

void Game::init() {
  InitUpgrades();
  InitWindow();
}

void Game::DrawUpgrades() {
  auto &i = UpgradeManager::instance();
  // TODO: Implement zoom
  Vector2 window_size = WindowSize();
  UpgradeVec fits((window_size.x / UPGRADE_SPACE.x) + 2,
                  (window_size.y / UPGRADE_SPACE.y) + 2);

  UpgradeVec cursor(0, 0);
  bool done = false;
  while (!done) {
    UpgradeVec screen_pos = cursor - (fits / 2.0f);
    UpgradeVec up_pos = screen_pos + m_pos;
    auto *up = i.GetUpgrade(up_pos);
    if (up) {

      Vector2 center = window_size / 2.0f;
      Vector2 draw_pos = center;
      draw_pos.x += screen_pos.x * UPGRADE_SPACE.x;
      draw_pos.y += screen_pos.y * UPGRADE_SPACE.y;
      draw_pos += m_offset;

      up->Draw(draw_pos);
    }

    if (cursor == fits) {
      done = true;
      break;
    }
    if (cursor.x >= fits.x) {
      cursor.x = 0;
      cursor.y++;
      continue;
    }
    cursor.x++;
  }
}

void Game::DrawAll() {
  BeginDrawing();
  ClearBackground(WHITE);

  DrawUpgrades();

  EndDrawing();
}

void Game::loop() {
  while (!m_end) {
    m_end = WindowShouldClose();

    DrawAll();
  }
}

void Game::end() { CloseWindow(); }

Vector2 Game::WindowSize() { return DEFAULT_WINDOW_SIZE; }
