#include "Text.hpp"
#include "Upgrades.hpp"
#include <memory>

using std::make_unique;

void RegisterUpgrades() {
  auto &i = UpgradeManager::instance();
  i.RegisterUpgrade(0, make_unique<Upgrade>(Desc(
                           Text("TEST", BLUE, RED), Text(" DESCRIPTION", RED),
                           Text("BLEBLEBLEBLEBLEBLEBLE", GREEN))));
}
