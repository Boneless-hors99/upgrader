#include "Text.hpp"
#include "imgui.h"
#include <sstream>
#include <string>
#include <utility>

std::string Text::str() { return m_str; }
const char *Text::chr() { return m_str.c_str(); }

ImVec2 Text::msr() { return msr(m_str); }

ImVec2 Text::msr(std::string str) { return ImGui::CalcTextSize(str.c_str()); }

bool Text::fits(float w) {
  std::istringstream t(m_str);
  std::string s, r;

  while (std::getline(t, s, ' ')) {
    if (msr(s).x > w)
      return false;
  }
  return true;
}

std::pair<Text, Text> Text::split(float w) {
  ImVec2 size = msr();
  if (size.x <= w) {
    return {*this, Text("")};
  }
  std::istringstream t(m_str);
  std::string s, r, e;
  bool enuf = false;

  while (std::getline(t, s, ' ')) {
    if (!enuf && msr(r + (r.empty() ? "" : " ") + s).x <= w) {
      if (!r.empty())
        r += ' ';
      r += s;
    } else {
      enuf = true;
      if (!e.empty())
        e += ' ';
      e += s;
    }
  }
  return {Text(r, m_col, m_backcol), Text(e, m_col, m_backcol)};
}

void Text::Render(ImVec2 pos) {
  // DrawTextPro(GetFontDefault(), chr(), pos, ImVec2(0.0f, 0.0f), 0.0f, 24.0f,
  //             1.0f, m_col);
  ImGui::SetCursorPos(pos);
  ImGui::PushStyleColor(ImGuiCol_Text, ImGui::ColorConvertFloat4ToU32(m_col));
  ImGui::Text("%s", m_str.c_str());
  ImGui::PopStyleColor();
}
