#pragma once

#include "Currency.hpp"
#include "Text.hpp"
#include "entt/entity/fwd.hpp"
#include <GL/gl.h>
#include <cstdint>
#include <fstream>
#include <imgui.h>
#include <unordered_map>
#include <vector>

#include <entt/entt.hpp>

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

  uint64_t i64() {
    return (static_cast<uint64_t>(x) << 32) | (static_cast<uint32_t>(y));
  }

  ImTextureID tex();

  int32_t x;
  int32_t y;

  UpgradeVec operator+(const UpgradeVec &v) const {
    return UpgradeVec(x + v.x, y + v.y);
  }
  UpgradeVec operator-(const UpgradeVec &v) const {
    return UpgradeVec(x - v.x, y - v.y);
  }

  ImVec2 operator*(const ImVec2 &v) const { return ImVec2(x * v.x, y * v.y); }

  UpgradeVec operator/(const float f) { return UpgradeVec(x / f, y / f); }
};

inline bool operator==(const UpgradeVec &v1, const UpgradeVec &v2) {
  return (v1.x == v2.x) && (v1.y == v2.y);
}

#define CONNECTIONS std::vector<UpgradeVec>

class Upgrade {
public:
  Upgrade(UpgradeVec p, Desc name, Price pr, Desc d, CONNECTIONS c)
      : m_pos(p), m_name(std::move(name)), m_price(pr),
        m_description(std::move(d)), m_connections(std::move(c)) {}
  void SetPos(UpgradeVec v) { m_pos = v; }

  bool Draw(ImVec2 pos, ImDrawList *list);

  bool Visible() { return m_visibility <= 1; }
  void SetVisibility(int vis) { m_visibility = vis; }

private:
  // STATIC MEMBERS
  UpgradeVec m_pos;
  Desc m_name;
  Price m_price;
  Desc m_description;
  // TODO: CONNECTIONS + DRAWING THEM
  CONNECTIONS m_connections;

  // DYNAMIC MEMBERS
  unsigned int m_visibility;
};

class UpgradeManager {
public:
  static UpgradeManager &instance();

  void init();

  entt::registry &GetRegistry() { return m_registry; }

  entt::entity RegisterUpgrade(UpgradeVec pos);
  entt::entity RegisterUpgrade(uint64_t id);

  bool HasUpgrade(UpgradeVec pos);
  entt::entity GetUpgrade(UpgradeVec pos);
  entt::entity GetUpgrade(uint64_t id);

private:
  entt::registry m_registry;
  std::unordered_map<uint64_t, entt::entity> m_upgrades;

  UpgradeManager();
};
