#include "Game.hpp"
#include "Currency.hpp"
#include "Text.hpp"
#include "Upgrades.hpp"

#include "User.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <chrono>
#include <iostream>
#include <thread>

void Game::InitUpgrades() {
  m_pos = UpgradeVec(0);
  m_offset = ImVec2(0.0f, 0.0f);
  auto &upgrades = UpgradeManager::instance();
  upgrades.init();
}

void Game::InitWindow() {
  if (!glfwInit())
    return;

  const char *glsl_version = "#version 130";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);

  // Create window with graphics context
  float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(
      glfwGetPrimaryMonitor()); // Valid on GLFW 3.3+ only
  m_window = glfwCreateWindow(1920, 1080, "upgrader", nullptr, nullptr);
  glfwSetWindowSize(m_window, 1920, 1080);
  if (m_window == nullptr)
    return;
  glfwMakeContextCurrent(m_window);
  glfwSwapInterval(1); // Enable vsync

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  // ImGui::StyleColorsLight();

  // Setup scaling
  ImGuiStyle &style = ImGui::GetStyle();
  style.ScaleAllSizes(
      main_scale); // Bake a fixed style scale. (until we have a solution for
                   // dynamic style scaling, changing this requires resetting
                   // Style + calling this again)
  style.FontScaleDpi =
      main_scale; // Set initial font scale. (in docking branch: using
                  // io.ConfigDpiScaleFonts=true automatically overrides this
                  // for every window depending on the current monitor)

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(m_window, true);

  ImGui_ImplOpenGL3_Init(glsl_version);
}

void Game::InitThreads() {
  m_threads[Threads::CURRENCY_GAINS] = std::thread([]() {
    while (GameState::instance().running) {
      Game::UpdateCurrencies(1.0f);
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  });
}

void Game::init() {
  GameState::instance().running = true;
  InitUpgrades();
  InitWindow();
  InitThreads();
  Settings::instance().SetMainTheme();
  Settings::instance().LoadFont();
}

void Game::UpdateCurrencies(float delta) {
  for (auto &gain : GameState::instance().currency_gains) {
    GameState::instance().currencies[gain.first] += gain.second() * delta;
  }
}

#ifdef DEBUG
void Game::DrawDebug() {

  ImGui::Begin("debug", nullptr, ImGuiWindowFlags_None);

  if (!ImGui::TreeNode("currencies"))
    goto style;

  for (auto &currency : GameState::instance().currencies) {

    ImGui::PushID((int)currency.first >> 1);
    currency.second.Selector();
    ImGui::SameLine();
    ImGui::Text("%s", CtoString(currency.first).c_str());
    ImGui::PopID();

    ImGui::PushID(((int)currency.first >> 1) + 1);
    CurrencyGain &gain = GameState::instance().currency_gains[currency.first];
    gain.base.Selector();
    ImGui::SameLine();
    ImGui::Text("/s");
    ImGui::PopID();
  }

  ImGui::TreePop();

style:
  if (!ImGui::TreeNode("style"))
    goto end;

  {
    auto &style = Settings::instance().theme;
    if (ImGui::Button("Apply"))
      Settings::instance().ApplyTheme();
    ImGui::InputFloat2("upgrade padding", (float *)&style.upgrade_padding);
    ImGui::InputFloat("upgrade rounding", &style.upgrade_rounding);
    ImGui::InputFloat("line thickness", &style.connection_thickness);
    ImGui::Checkbox("upgrade randomness", &style.upgrade_random);
    ImGui::Separator();
    ImGui::InputFloat("description rounding", &style.desc_rounding);
    ImGui::InputFloat("text rounding", &style.text_rounding);
    ImGui::InputFloat2("description padding", (float *)&style.desc_padding);
    ImGui::InputFloat2("text padding", (float *)&style.text_padding);
    ImGui::Separator();
    ImGui::InputFloat("font size", &style.font_size);
  }

  ImGui::TreePop();

end:
  ImGui::End();
}
#endif // DEBUG

void Game::DrawUpgrades() {
  auto &i = UpgradeManager::instance();
  // TODO: Implement zoom
  ImVec2 window_size = WindowSize();
  UpgradeVec fits((window_size.x / UPGRADE_SPACE.x) + 2,
                  (window_size.y / UPGRADE_SPACE.y) + 2);

  UpgradeVec cursor(0, 0);
  bool done = false;

  auto &reg = i.GetRegistry();

  ImDrawList *list = ImGui::GetWindowDrawList();
  list->ChannelsSplit(5);
  ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding,
                      Settings::instance().theme.upgrade_rounding);
  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 5.0f));

  while (!done) {

    UpgradeVec screen_pos = cursor - (fits / 2.0f);
    UpgradeVec up_pos = screen_pos + m_pos;

    if (i.HasUpgrade(up_pos)) {

      auto ent = i.GetUpgrade(up_pos);
      auto &up = reg.get<Upgrade>(ent);
      ImVec2 center = window_size / 2.0f;
      ImVec2 draw_pos = center;
      draw_pos.x += screen_pos.x * UPGRADE_SPACE.x;
      draw_pos.y += screen_pos.y * UPGRADE_SPACE.y;
      draw_pos += m_offset;

      auto out = up.Draw(draw_pos, list);

      if (reg.all_of<UpgradeDrawer>(ent)) {
        UpgradeDrawer drawer = reg.get<UpgradeDrawer>(ent);
        if (drawer.flags & out.second) {
          list->ChannelsSetCurrent(2);
          drawer.draw(center);
        }
      }

      if (out.first && up.CanBuy()) {
        up.Buy();
      }
    }

    if (cursor == fits) {

      done = true;
      break;
    }
    if (cursor.x >= fits.x) {

      cursor.x = 0;
      cursor.y++;
      continue;
    }

    cursor.x++;
  }

  ImGui::PopStyleVar(2);
  list->ChannelsMerge();
}

void Game::DrawAll() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();

  ImGui::NewFrame();
  ImGui::PushFont(Settings::instance().font,
                  Settings::instance().theme.font_size);

  ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
  ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
  ImGui::Begin("back", NULL,
               ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                   ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                   ImGuiWindowFlags_NoScrollWithMouse |
                   ImGuiWindowFlags_NoBringToFrontOnFocus);

  DrawUpgrades();

  ImGui::End();

#ifdef DEBUG
  DrawDebug();
#endif // DEBUG

  ImGui::PopFont();
  ImGui::Render();
  int display_w, display_h;
  glfwGetFramebufferSize(m_window, &display_w, &display_h);
  glViewport(0, 0, display_w, display_h);
  glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w,
               clear_color.z * clear_color.w, clear_color.w);
  glClear(GL_COLOR_BUFFER_BIT);
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  glfwSwapBuffers(m_window);
}

void Game::loop() {
  while (GameState::instance().running) {
    glfwPollEvents();
    GameState::instance().running = !glfwWindowShouldClose(m_window);

    DrawAll();
  }
}

void Game::end() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(m_window);
  glfwTerminate();

  for (auto &thread : m_threads) {
    thread.second.join();
  }
}

ImVec2 Game::WindowSize() { return DEFAULT_WINDOW_SIZE; }
