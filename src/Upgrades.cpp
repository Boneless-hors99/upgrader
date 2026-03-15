#include "Upgrades.hpp"
#include <cstdint>
#include <memory>
#include <raylib.h>

void Upgrade::Draw(Vector2 pos) {
  Rectangle rec(pos.x, pos.y, UPGRADE_SIZE.x, UPGRADE_SIZE.y);
  Texture2D tex = m_pos.tex();
  if (tex.height == 0) {
    TraceLog(LOG_ERROR, "NOT LOADING TEXTURE");
    return;
  }
  DrawTexturePro(tex, Rectangle(0.0f, 0.0f, tex.width, tex.height), rec,
                 UPGRADE_SIZE / 2.0f, 0.0f, WHITE);
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
