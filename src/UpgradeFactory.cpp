#include "Text.hpp"
#include "Upgrades.hpp"

void RegisterUpgrades() {
  auto &i = UpgradeManager::instance();
  auto &r = i.GetRegistry();
  entt::entity upgrade;

  upgrade = i.RegisterUpgrade(0);
  r.emplace<Upgrade>(upgrade, Desc(Text("ENTT WORKING!!!")));
}
