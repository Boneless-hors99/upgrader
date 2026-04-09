#include "Currency.hpp"
#include "Game.hpp"
#include "Text.hpp"
#include "Upgrades.hpp"
#include "User.hpp"
#include "imgui.h"
#include <functional>

static entt::entity AddUpgrade(auto &mgr, const UpgradeVec &pos, Desc name,
                               Price price, Desc desc,
                               CONNECTIONS connections) {
  auto entity = mgr.RegisterUpgrade(pos);
  mgr.GetRegistry().template emplace<Upgrade>(
      entity, std::move(pos), std::move(name), std::move(price),
      std::move(desc), std::move(connections));
  return entity;
}

void DrawCurrencyBoard(ImVec2 pos, float w, Currencies c, TextColors cols) {
  Desc d(Settings::instance().theme.secondary,
         Text(GameState::instance().currencies[c].toString() + CtoString(c) +
                  (GameState::instance().currency_gains.contains(c)
                       ? GameState::instance().currency_gains[c].toString(true)
                       : ""),
              cols));
  pos.y -= ImGui::GetTextLineHeight();
  d.Render(pos, w);
}

void RegisterUpgrades() {
  auto &i = UpgradeManager::instance();

  auto e = AddUpgrade(i, UpgradeVec(0, 0), Desc(Text("the first")), Price(0.0f),
                      Desc(Text("begin gaining "), Text("+1X/s", XColors)),
                      {UpgradeVec(0, -1)});
  i.GetRegistry().emplace<UpgradeDrawer>(
      e,
      [](ImVec2 v) {
        DrawCurrencyBoard(ImVec2(v.x, v.y - UPGRADE_SPACE.y / 2.0f), 100.0f,
                          Currencies::Currency_X, XColors);
      },
      UpgradeDrawerFlag::NeedPurchase);
  i.GetRegistry().emplace<UpgradeBuyer>(e, []() {
    GameState::instance().currencies[Currencies::Currency_X] = 0.0f;
    GameState::instance().currency_gains[Currencies::Currency_X].base = 1.0f;
  });

  e = AddUpgrade(i, UpgradeVec(0, -1), Desc(Text("the second")), Price(10.0f),
                 Desc(Text("*2X/s", XColors)), {UpgradeVec(1, -1)});

  i.GetRegistry().emplace<UpgradeBuyer>(e, []() {
    GameState::instance().currency_gains[Currencies::Currency_X] *= 2;
  });

  AddUpgrade(i, UpgradeVec(1, -1), Desc(Text("the third")), Price(30.0f),
             Desc(Text("x")), {});
}
