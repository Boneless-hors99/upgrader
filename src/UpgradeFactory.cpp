#include "Currency.hpp"
#include "Upgrades.hpp"

#include <print>

static void AddUpgrade(auto &mgr, const UpgradeVec &pos, Price price, Desc desc,
                       CONNECTIONS connections) {
  auto entity = mgr.RegisterUpgrade(pos);
  mgr.GetRegistry().template emplace<Upgrade>(entity, std::move(pos),
                                              std::move(price), std::move(desc),
                                              std::move(connections));
}

void RegisterUpgrades() {
  auto &i = UpgradeManager::instance();

  AddUpgrade(i, {0, 0}, Price(0.0f), Desc(Text("ENTT WORKING!!!")),
             {UpgradeVec(0, -1)});

  AddUpgrade(i, UpgradeVec(0, -1), Price(10.0f), Desc(Text("ENTT WORKING!!!")),
             {});
}
