#include "Upgrades.hpp"
#include "Game.hpp"
#include "PerlinNoise.hpp"
#include "Text.hpp"
#include "User.hpp"
#include "entt/container/dense_set.hpp"
#include "entt/entity/fwd.hpp"
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <cstdint>
#include <imgui.h>
#include <iostream>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

float Smoothstep(float x) { return 3 * x * x - 2 * x * x * x; }

const ImVec2 UpgradeSize(bool include_border, float time_hovered) {
  const float hover_time = 0.12f;
  const float size_change = 0.0f;
  if (time_hovered > 0.0f && size_change >= 0.0f) {
    float time =
        std::clamp((float)glfwGetTime() - time_hovered, 0.0f, hover_time);
    ImVec2 true_size =
        UPGRADE_SIZE *
        (1 + Smoothstep(time * (1.0f / hover_time)) * size_change);
    return include_border
               ? true_size + Settings::instance().theme.upgrade_padding * 2.0f
               : true_size;
  }
  return include_border
             ? UPGRADE_SIZE + Settings::instance().theme.upgrade_padding * 2.0f
             : UPGRADE_SIZE;
}

const std::pair<ImVec2, ImVec2> UpgradePos(ImVec2 start_pos,
                                           float time_hovered) {
  const float hover_time = 0.15f;
  const float pos_change = 10.0f;
  const float random_change = 10.0f;
  if (time_hovered > 0.0f) {
    static siv::PerlinNoise perlin{123456u};
    float time =
        std::clamp((float)glfwGetTime() - time_hovered, 0.0f, hover_time);
    ImVec2 true_pos = start_pos;
    true_pos.y -= pos_change * Smoothstep(time * (1.0 / hover_time));
    ImVec2 norm_pos = true_pos;
    if (!Settings::instance().theme.upgrade_random) {
      return {true_pos, norm_pos};
    }
    true_pos.y += perlin.noise1D(glfwGetTime()) * random_change;
    true_pos.x += perlin.noise1D(glfwGetTime() + 100.0) * random_change;
    return {true_pos, norm_pos};
  }
  return {start_pos, start_pos};
}

ImTextureID UpgradeVec::tex() {
  uint64_t key = i64();

  // already loaded?
  auto it = textureCache.find(key);
  if (it != textureCache.end()) {
    std::cout << "NO TEXTURE FOUND" << std::endl;
    return it->second;
  }

  PackEntry *e = FindEntry(key);
  if (!e) {
    std::cout << "NO TEXTURE FOUND" << std::endl;
    return 0;
  }

  std::vector<unsigned char> buffer(e->size);

  packFile.seekg(e->offset);
  packFile.read((char *)buffer.data(), e->size);

  int image_width = 0;
  int image_height = 0;
  unsigned char *image_data = stbi_load_from_memory(
      buffer.data(), buffer.size(), &image_width, &image_height, NULL, 4);
  if (image_data == NULL)
    return false;

  // Create a OpenGL texture identifier
  GLuint image_texture;
  glGenTextures(1, &image_texture);
  glBindTexture(GL_TEXTURE_2D, image_texture);

  // Setup
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  // Upload pixels into texture
  glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, image_data);
  stbi_image_free(image_data);

  return (ImTextureID)(intptr_t)image_texture;

  return 0;
}

std::pair<bool, UpgradeFlags> Upgrade::Draw(ImVec2 pos, ImDrawList *list) {

  ImTextureID tex = m_pos.tex();
  if (!tex) {
    return {false, 0};
  }

  static bool hovering = false;
  static UpgradeVec hovered(0);
  static float hover_time = 0.0f;

  list->ChannelsSetCurrent(0);
  for (const auto connection : m_connections) {
    ImVec2 lpos = pos;
    lpos.x -= 0.5f; // Because of the upgrades being even in size
    ImVec2 next_pos = lpos + (connection - m_pos) * UPGRADE_SPACE;
    list->AddLine(lpos, next_pos, Col32(TextColor::WHITE),
                  Settings::instance().theme.connection_thickness);
  }

  list->ChannelsSetCurrent(1);

  UpgradeFlags flags = 0;
  ImVec4 UpBgCol = ColV4(TextColor::BLACK);
  if (m_bought) {
    ImGui::PushStyleVar(ImGuiStyleVar_DisabledAlpha, 1.0f);
    ImGui::PushStyleColor(ImGuiCol_Button,
                          DarkenV4(Settings::instance().theme.main));
    ImGui::BeginDisabled();
    flags |= UpgradeDrawerFlag::NeedPurchase;
  } else {
    ImGui::PushStyleColor(ImGuiCol_Button,
                          ColV4(Settings::instance().theme.main));
  }

  float time = hovering && hovered == m_pos ? hover_time : 0.0f;
  auto true_pos = UpgradePos(pos, time);
  ImGui::SetCursorPos(true_pos.first - UpgradeSize(true, time) / 2.0f);
  bool clicked =
      ImGui::ImageButton(std::to_string(m_pos.i64()).c_str(), ImTextureRef(tex),
                         UpgradeSize(false, time), ImVec2(0.0f, 0.0f),
                         ImVec2(1.0f, 1.0f), UpBgCol);

  ImGui::PopStyleColor();
  if (m_bought) {
    ImGui::EndDisabled();
    ImGui::PopStyleVar();
  }
  if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {

    hovering = true;
    hovered = m_pos;
    if (hover_time == 0.0f) {
      hover_time = glfwGetTime();
    }
    list->ChannelsSetCurrent(4);

    ImVec2 descpos(true_pos.second.x,
                   true_pos.second.y + UpgradeSize(true, time).y / 1.8f);
    ImVec2 backpos(descpos);
    ImVec2 backsize(200.0f, 0.0f);

    auto lower_desc = [&descpos, &backsize](float h) {
      descpos.y += h;
      backsize.y += h;
    };

    auto bcol = TextColor::NONE;
    lower_desc(ImGui::GetTextLineHeight() / 2.0f);
    lower_desc(m_name.Render(descpos, 200.0f, true, bcol));
    lower_desc(ImGui::GetTextLineHeight());
    lower_desc(m_description.Render(descpos, 200.0f, true, bcol));
    lower_desc(ImGui::GetTextLineHeight());
    Desc price(m_price.toText());
    lower_desc(price.Render(descpos, 200.0f, true, bcol));
    lower_desc(ImGui::GetTextLineHeight());

    list->ChannelsSetCurrent(3);

    DrawDescBackground(backpos, backsize, TextColor::BLACK,
                       Settings::instance().theme.main);

    flags |= UpgradeDrawerFlag::NeedHover;

  } else if (hovered == m_pos) {
    hovering = false;
    hover_time = 0.0f;
  }
  flags |= flags & UpgradeDrawerFlag::NeedActive
               ? 0
               : UpgradeDrawerFlag::NeedDisabled;
  return {clicked, flags};
}

bool Upgrade::CanBuy() {
  return m_price.amt == 0.0f ||
         GameState::instance().currencies[m_price.cur] >= m_price.amt;
}

void Upgrade::Buy() {
  if (m_price.amt > 0.0f)
    GameState::instance().currencies[m_price.cur] -= m_price.amt;
  auto &i = UpgradeManager::instance();
  if (i.GetRegistry().all_of<UpgradeBuyer>(i.GetUpgrade(m_pos)))
    i.GetRegistry().get<UpgradeBuyer>(i.GetUpgrade(m_pos)).buy();
  m_bought = true;
}

UpgradeManager &UpgradeManager::instance() {
  static UpgradeManager inst;
  return inst;
}

void UpgradeManager::init() {
  RegisterUpgrades();
  LoadPack("upgrades.bones");
}

entt::entity UpgradeManager::RegisterUpgrade(UpgradeVec pos) {
  return RegisterUpgrade(pos.i64());
}

entt::entity UpgradeManager::RegisterUpgrade(uint64_t id) {
  const auto entity = m_registry.create();
  m_upgrades[id] = std::move(entity);
  return entity;
}

bool UpgradeManager::HasUpgrade(UpgradeVec pos) {
  return m_upgrades.contains(pos.i64());
}

entt::entity UpgradeManager::GetUpgrade(UpgradeVec pos) {
  return GetUpgrade(pos.i64());
}

entt::entity UpgradeManager::GetUpgrade(uint64_t id) {
  auto it = m_upgrades.find(id);
  if (it == m_upgrades.end())
    return entt::entity();
  return it->second;
}

UpgradeManager::UpgradeManager() { m_upgrades.reserve(100); }
