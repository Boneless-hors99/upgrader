#include "Text.hpp"
#include "User.hpp"
#include "imgui.h"
#include <sstream>
#include <string>
#include <utility>

using enum TextColor;

ImVec4 DarkenV4(ImVec4 col) {
  ImVec4 out = col;
  out.x /= 3.0f;
  out.y /= 3.0f;
  out.z /= 3.0f;
  return out;
}

ImU32 Col32(TextColor col) {
  return ImGui::ColorConvertFloat4ToU32(ColV4(col));
}

ImVec4 ColV4(TextColor col) {
  switch (col) {
  case BLACK:
    return ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
  case WHITE:
    return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
  case LIGHT_GREY:
    return ImVec4(0.85f, 0.85f, 0.85f, 1.0f);
  case DARK_GREY:
    return ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
  case RED:
    return ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
  case GREEN:
    return ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
  case BLUE:
    return ImVec4(0.0f, 0.0f, 1.0f, 1.0f);
  case NONE:
    return ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
  }
}

ImVec4 DarkenV4(TextColor col) { return DarkenV4(ColV4(col)); }

std::string Text::str() const { return m_str; }
const char *Text::chr() { return m_str.c_str(); }

ImVec2 Text::msr() const { return msr(m_str); }

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

void Text::Render(ImVec2 pos) const {
  // DrawTextPro(GetFontDefault(), chr(), pos, ImVec2(0.0f, 0.0f), 0.0f, 24.0f,
  //             1.0f, m_col);
  ImGui::SetCursorPos(pos);
  ImGui::PushStyleColor(ImGuiCol_Text, Col32(m_col));

  if (m_backcol != NONE) {
    // TODO: PADDING
    ImGui::GetWindowDrawList()->AddRectFilled(
        pos - Settings::instance().theme.text_padding,
        pos + msr() + Settings::instance().theme.text_padding, Col32(m_backcol),
        Settings::instance().theme.text_rounding);
  }

  // TODO: NEW FONT & SIZE
  ImGui::Text("%s", m_str.c_str());

  ImGui::PopStyleColor();
}

void DrawDescBackground(ImVec2 pos, ImVec2 size, TextColor col,
                        TextColor bordercolor) {
  auto *list = ImGui::GetWindowDrawList();

  ImVec2 start = pos;
  start.x -= size.x / 2.0f;

  ImVec2 end = pos;
  end.y += size.y;
  end.x += size.x / 2.0f;
  float rounding = Settings::instance().theme.desc_rounding;

  if (bordercolor != NONE) {
    list->AddRectFilled(start - Settings::instance().theme.desc_padding,
                        end + Settings::instance().theme.desc_padding,
                        Col32(bordercolor), rounding);
    rounding -= Settings::instance().theme.desc_padding.x;
  }

  list->AddRectFilled(
      start, end, Col32(col == NONE ? Settings::instance().theme.main : col),
      Settings::instance().theme.desc_rounding);
}
