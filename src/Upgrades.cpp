#include "Upgrades.hpp"
#include <cstdint>
#include <memory>
#include <raylib.h>

bool Upgrade::Draw(Vector2 pos, Vector2 mouse_pos) {
  Rectangle trec(pos.x, pos.y, UPGRADE_SIZE.x, UPGRADE_SIZE.y);
  const float UPGRADE_PADDING = 16.0f;
  Rectangle brec(pos.x - UPGRADE_SIZE.x / 2.0f - UPGRADE_PADDING / 2.0f,
                 pos.y - UPGRADE_SIZE.y / 2.0f - UPGRADE_PADDING / 2.0f,
                 UPGRADE_SIZE.x + UPGRADE_PADDING,
                 UPGRADE_SIZE.y + UPGRADE_PADDING);
  DrawRectangleRounded(brec, 0.05f, 10, GREEN);
  Texture2D tex = m_pos.tex();
  if (tex.height == 0) {
    TraceLog(LOG_ERROR, "NOT LOADING TEXTURE");
    return false;
  }
  DrawTexturePro(tex, Rectangle(0.0f, 0.0f, tex.width, tex.height), trec,
                 UPGRADE_SIZE / 2.0f, 0.0f, WHITE);
  if (CheckCollisionPointRec(mouse_pos, brec)) {
    m_description.Render(
        Vector2(brec.x + brec.width / 2.0f, brec.y + brec.height), 200.0f);
    return true;
  }
  return false;
}

UpgradeManager &UpgradeManager::instance() {
  static UpgradeManager inst;
  return inst;
}

void UpgradeManager::init() {
  RegisterUpgrades();
  LoadPack("upgrades.bones");
}

void UpgradeManager::RegisterUpgrade(UpgradeVec pos,
                                     std::unique_ptr<Upgrade> u) {
  u->SetPos(pos);
  RegisterUpgrade(pos.i64(), std::move(u));
}

void UpgradeManager::RegisterUpgrade(uint64_t id, std::unique_ptr<Upgrade> u) {
  upgrades[id] = std::move(u);
}

Upgrade *UpgradeManager::GetUpgrade(UpgradeVec pos) {
  return GetUpgrade(pos.i64());
}

Upgrade *UpgradeManager::GetUpgrade(uint64_t id) {
  auto it = upgrades.find(id);
  if (it == upgrades.end())
    return nullptr;
  return it->second.get();
}

UpgradeManager::UpgradeManager() { upgrades.reserve(100); }
