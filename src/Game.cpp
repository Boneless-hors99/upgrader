#include "Game.hpp"
#include "Upgrades.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

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

void Game::init() {
  InitUpgrades();
  InitWindow();
}

void Game::DrawUpgrades() {
  auto &i = UpgradeManager::instance();
  // TODO: Implement zoom
  ImVec2 window_size = WindowSize();
  ImVec2 mouse_pos = ImGui::GetCursorScreenPos();
  UpgradeVec fits((window_size.x / UPGRADE_SPACE.x) + 2,
                  (window_size.y / UPGRADE_SPACE.y) + 2);

  UpgradeVec cursor(0, 0);
  bool done = false;

  auto &reg = i.GetRegistry();

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

      up.Draw(draw_pos, mouse_pos);
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
}

void Game::DrawAll() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
  ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
  ImGui::Begin("back", NULL,
               ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                   ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                   ImGuiWindowFlags_NoScrollWithMouse |
                   ImGuiWindowFlags_NoBringToFrontOnFocus);

  DrawUpgrades();

  ImGui::End();

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
  while (!m_end) {
    glfwPollEvents();
    m_end = glfwWindowShouldClose(m_window);

    DrawAll();
  }
}

void Game::end() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(m_window);
  glfwTerminate();
}

ImVec2 Game::WindowSize() { return DEFAULT_WINDOW_SIZE; }
