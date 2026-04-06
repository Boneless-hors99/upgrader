#include "Currency.hpp"
#include "Text.hpp"
#include "Upgrades.hpp"

#include <algorithm>
#include <print>

static void AddUpgrade(auto &mgr, const UpgradeVec &pos, Desc name, Price price,
                       Desc desc, CONNECTIONS connections) {
  auto entity = mgr.RegisterUpgrade(pos);
  mgr.GetRegistry().template emplace<Upgrade>(
      entity, std::move(pos), std::move(name), std::move(price),
      std::move(desc), std::move(connections));
}

void RegisterUpgrades() {
  auto &i = UpgradeManager::instance();

  AddUpgrade(i, UpgradeVec(0, 0), Desc(Text("the first")), Price(0.0f),
             Desc(Text("ENTT WORKING!!!")), {UpgradeVec(0, -1)});

  AddUpgrade(i, UpgradeVec(0, -1), Desc(Text("the second")), Price(10.0f),
             Desc(Text("ENTT WORKING!!!")), {});
}
