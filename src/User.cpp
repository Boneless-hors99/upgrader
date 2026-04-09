#include "User.hpp"
#include "Text.hpp"
#include "imgui.h"

void Settings::SetTheme(GameTheme theme) {
  this->theme = theme;
  ApplyTheme();
}

void Settings::ApplyTheme() {
  using enum TextColor;
  auto &style = ImGui::GetStyle();
  style.WindowRounding = 0.0f;
  style.FramePadding = theme.upgrade_padding;
  style.FrameRounding = theme.upgrade_rounding;
  style.ScrollbarRounding = 0.0f;
  style.Colors[ImGuiCol_WindowBg] = ColV4(BLACK);
  style.Colors[ImGuiCol_ChildBg] = ColV4(BLACK);
  style.Colors[ImGuiCol_Border] = ColV4(theme.main);
  style.Colors[ImGuiCol_BorderShadow] = ColV4(theme.secondary);
  style.Colors[ImGuiCol_TitleBg] = ColV4(theme.secondary);
  style.Colors[ImGuiCol_TitleBgActive] = ColV4(theme.secondary);
  style.Colors[ImGuiCol_MenuBarBg] = ColV4(theme.secondary);
  style.Colors[ImGuiCol_Button] = ColV4(theme.secondary);
}

void Settings::SetMainTheme() {
  using enum TextColor;
  GameTheme theme{
      WHITE, DARK_GREY,          ImVec2(5.0f, 5.0f), 2.0f, 4.0f, true, 10.0f,
      6.0f,  ImVec2(4.0f, 4.0f), ImVec2(4.0f, 2.0f), 17.0f};
  SetTheme(theme);
}

void Settings::LoadFont() {
  ImGuiIO &io = ImGui::GetIO();
  ImFontConfig cfg;
  cfg.OversampleH = 2;
  cfg.OversampleV = 1;
  cfg.PixelSnapH = true;
  font = io.Fonts->AddFontFromFileTTF("JetBrainsMonoNerdFont-Bold.ttf", 0.0f,
                                      &cfg);
}
