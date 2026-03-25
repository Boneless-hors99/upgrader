#include "Upgrades.hpp"
#include "entt/entity/fwd.hpp"
#include <GL/gl.h>
#include <cstdint>
#include <imgui.h>
#include <iostream>
#include <memory>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

ImTextureID UpgradeVec::tex() {
  uint64_t key = i64();

  // already loaded?
  auto it = textureCache.find(key);
  if (it != textureCache.end())
    return it->second;

  PackEntry *e = FindEntry(key);
  if (!e)
    return 0;

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

bool Upgrade::Draw(ImVec2 pos, ImVec2 mouse_pos) {
  ImGui::SetCursorPos(pos - UPGRADE_SIZE / 2.0f);
  ImTextureID tex = m_pos.tex();
  if (!tex) {
    return false;
  }
  ImGui::ImageButton(std::to_string(m_pos.i64()).c_str(), ImTextureRef(tex),
                     UPGRADE_SIZE);
  if (ImGui::IsItemHovered()) {
    // TODO: TITLE & TYPES (CHECK BALATRO)
    m_description.Render(ImVec2(pos.x, pos.y + UPGRADE_SIZE.y / 1.6f), 200.0f);
    return true;
  }
  return false;
  /*
  Rectangle trec(pos.x, pos.y, UPGRADE_SIZE.x, UPGRADE_SIZE.y);
  const float UPGRADE_PADDING = 16.0f;
  Rectangle brec(pos.x - UPGRADE_SIZE.x / 2.0f - UPGRADE_PADDING / 2.0f,
                 pos.y - UPGRADE_SIZE.y / 2.0f - UPGRADE_PADDING / 2.0f,
                 UPGRADE_SIZE.x + UPGRADE_PADDING,
                 UPGRADE_SIZE.y + UPGRADE_PADDING);
  DrawRectangleRounded(brec, 0.05f, 10, GREEN);
  ImTextureID tex = m_pos.tex();
  if (tex.height == 0) {
    return false;
  }
  DrawTexturePro(tex, Rectangle(0.0f, 0.0f, tex.width, tex.height), trec,
                 UPGRADE_SIZE / 2.0f, 0.0f, WHITE);
  if (CheckCollisionPointRec(mouse_pos, brec)) {
    m_description.Render(
        ImVec2(brec.x + brec.width / 2.0f, brec.y + brec.height), 200.0f);
    return true;
  }
  return false;
  */
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
