#include "Upgrades.hpp"
#include "Text.hpp"
#include "entt/entity/fwd.hpp"
#include <GL/gl.h>
#include <cstdint>
#include <imgui.h>
#include <iostream>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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

  /*
  Image img = LoadImageFromMemory(".png", buffer.data(), buffer.size());
  Texture2D tex = LoadTextureFromImage(img);
  UnloadImage(img);

  textureCache[key] = tex;

  return tex;
  */

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

  // Setup filtering parameters for display
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

bool Upgrade::Draw(ImVec2 pos, ImDrawList *list) {
  ImGui::SetCursorPos(pos - UPGRADE_SIZE / 2.0f);

  ImTextureID tex = m_pos.tex();
  if (!tex) {
    return false;
  }

  list->ChannelsSetCurrent(0);
  for (const auto connection : m_connections) {
    ImVec2 lpos = pos;
    lpos.x += 4.0f;
    ImVec2 next_pos = lpos + (connection - m_pos) * UPGRADE_SPACE;
    list->AddLine(lpos, next_pos, Col32(TextColor::WHITE), 4.0f);
  }

  list->ChannelsSetCurrent(1);
  ImGui::ImageButton(std::to_string(m_pos.i64()).c_str(), ImTextureRef(tex),
                     UPGRADE_SIZE);

  if (ImGui::IsItemHovered()) {
    // TODO: TITLE & TYPES (CHECK BALATRO)
    ImVec2 descpos(pos.x, pos.y + UPGRADE_SIZE.y / 1.6f);
    descpos.y += m_description.Render(descpos, 200.0f);
    Desc price(m_price.toText());
    price.Render(descpos, 200.0f);
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
