#pragma once

#include "Text.hpp"
#include <GL/gl.h>
#include <cstdint>
#include <fstream>
#include <imgui.h>
#include <memory>
#include <unordered_map>
#include <vector>

struct PackEntry {
  uint64_t key;
  uint64_t offset;
  uint32_t size;
};

static std::ifstream packFile;
static std::vector<PackEntry> packTable;
static uint64_t tableSize;

inline void LoadPack(const char *path) {
  packFile.open(path, std::ios::binary);

  uint64_t magic;
  packFile.read((char *)&magic, sizeof(magic));
  packFile.read((char *)&tableSize, sizeof(tableSize));

  packTable.resize(tableSize);
  packFile.read((char *)packTable.data(), sizeof(PackEntry) * tableSize);
}

inline PackEntry *FindEntry(uint64_t key) {
  uint64_t index = key % tableSize;

  for (;;) {
    PackEntry &e = packTable[index];

    if (e.size == 0)
      return nullptr;

    if (e.key == key)
      return &e;

    index = (index + 1) % tableSize;
  }
}

static std::unordered_map<uint64_t, ImTextureID> textureCache;

const ImVec2 UPGRADE_SIZE(64.0f, 64.0f);
const ImVec2 UPGRADE_SPACE(UPGRADE_SIZE * 2.5f);

void RegisterUpgrades();

struct UpgradeVec {
  UpgradeVec(int32_t x, uint32_t y) : x(x), y(y) {}
  UpgradeVec(uint64_t i) {
    x = static_cast<int32_t>(i & uint32_t(-1));
    y = static_cast<int32_t>(i >> 32);
  }

  uint64_t i64() { return (static_cast<uint64_t>(x) << 32) | y; }

  ImTextureID tex();

  int32_t x;
  int32_t y;

  UpgradeVec operator+(const UpgradeVec &v) {
    return UpgradeVec(x + v.x, y + v.y);
  }
  UpgradeVec operator-(const UpgradeVec &v) {
    return UpgradeVec(x - v.x, y - v.y);
  }

  UpgradeVec operator/(const float f) { return UpgradeVec(x / f, y / f); }
};

inline bool operator==(const UpgradeVec &v1, const UpgradeVec &v2) {
  return (v1.x == v2.x) && (v1.y == v2.y);
}

class Upgrade {
public:
  Upgrade(Desc d) : m_pos(0), m_description(std::move(d)) {}
  void SetPos(UpgradeVec v) { m_pos = v; }

  bool Draw(ImVec2 pos, ImVec2 mouse_pos);

private:
  UpgradeVec m_pos;
  Desc m_description;
};

class UpgradeManager {
public:
  static UpgradeManager &instance();

  void init();

  void RegisterUpgrade(UpgradeVec pos, std::unique_ptr<Upgrade> u);
  void RegisterUpgrade(uint64_t id, std::unique_ptr<Upgrade> u);

  Upgrade *GetUpgrade(UpgradeVec pos);
  Upgrade *GetUpgrade(uint64_t id);

private:
  std::unordered_map<uint64_t, std::unique_ptr<Upgrade>> upgrades;

  UpgradeManager();
};
