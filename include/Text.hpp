#pragma once
#include <algorithm>
#include <utility>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <memory>
#include <print>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

/*
#define BLACK ImVec4(0.0f, 0.0f, 0.0f, 1.0f)
#define WHITE ImVec4(1.0f, 1.0f, 1.0f, 1.0f)
#define LIGHT_GREY ImVec4(0.85f, 0.85f, 0.85f, 1.0f)
#define RED ImVec4(1.0f, 0.0f, 0.0f, 1.0f)
#define GREEN ImVec4(0.0f, 1.0f, 0.0f, 1.0f)
#define BLUE ImVec4(0.0f, 0.0f, 1.0f, 1.0f)
*/

enum class TextColor {
  BLACK,
  WHITE,
  LIGHT_GREY,
  DARK_GREY,
  RED,
  GREEN,
  BLUE,
  NONE
};

using TextColors = std::pair<TextColor, TextColor>;
const TextColors XColors(TextColor::WHITE, TextColor::BLACK);

ImVec4 DarkenV4(ImVec4 col);

ImU32 Col32(TextColor col);
ImVec4 ColV4(TextColor col);
ImVec4 DarkenV4(TextColor col);

class Text {
public:
  Text(std::string_view str, TextColor col = TextColor::BLACK,
       TextColor backcol = TextColor::NONE)
      : m_str(str), m_col(col), m_backcol(backcol) {}
  Text(std::string_view str, TextColors cols)
      : m_str(str), m_col(cols.first), m_backcol(cols.second) {}

  std::string str() const;
  const char *chr();

  ImVec2 msr() const;
  static ImVec2 msr(std::string str);

  bool fits(float w);

  std::pair<Text, Text> split(float w);

  void Render(ImVec2 pos) const;

private:
  std::string m_str;
  TextColor m_col;
  TextColor m_backcol;
};

void DrawDescBackground(ImVec2 pos, ImVec2 size,
                        TextColor col = TextColor::NONE,
                        TextColor border_col = TextColor::NONE);

#define LINES std::vector<Desc>

class Desc {
public:
  Desc(const Desc &) = delete;
  Desc &operator=(const Desc &) = delete;

  Desc(Desc &&) = default;
  Desc &operator=(Desc &&) = default;

  template <typename... Args>
    requires(std::is_base_of_v<Text, std::decay_t<Args>> && ...)
  explicit Desc(TextColor col, Args &&...args) : m_border(col) {
    (Append(Text(std::forward<Args>(args))), ...);
  }

  template <typename... Args>
    requires(std::is_base_of_v<Text, std::decay_t<Args>> && ...)
  explicit Desc(Args &&...args) : m_border(TextColor::NONE) {
    (Append(Text(std::forward<Args>(args))), ...);
  }

  explicit Desc(std::string s) : m_border(TextColor::NONE) { Append(Text(s)); }

  // Returns total width of texts in description
  float msr() {
    float sum{};
    for (const auto &t : m_text)
      sum += t.msr().x;
    return sum;
  }

  static float AddText(LINES *lines, Text text, float maxwidth) {

    if (!text.fits(maxwidth)) {
      return -1;
    }

    int line;
    bool finished = false;
    float max_w = 0.0f;
    while (!finished) {
      line = static_cast<int>(lines->size()) - 1;
      float occupied = (*lines)[line].msr();
      float space = maxwidth - occupied;

      auto splitted = text.split(space);

      if (splitted.first.str().empty())
        goto nextline;

      (*lines)[line].Append(splitted.first);
      max_w = std::max(max_w, (*lines)[line].msr());

      if (splitted.second.str().empty()) {
        break;
      }

      text = splitted.second;

    nextline:
      lines->push_back(Desc(Text("")));
      line++;
    }
    return max_w;
  }

  void RenderLine(ImVec2 pos) {
    ResetCursor();
    float s = msr();
    m_cursor.x -= s / 2.0f;
    for (const auto &t : m_text) {
      ImVec2 tpos = pos;
      t.Render(tpos + m_cursor);
      m_cursor.x += t.msr().x;
    }
  }

  float Render(ImVec2 pos, float w, bool fit_background = true,
               TextColor border_col = TextColor::NONE) {
    ResetCursor();
    LINES lines;
    lines.emplace_back();
    float line = 0;
    float max_w = 0.0f;

    // Turns this description into LINES with each being maximum w long
    for (const auto &t : m_text) {
      line = AddText(&lines, t, w);
      if (line < 0.0f)
        goto overflow;
      max_w = std::max(max_w, line);
    }

    // *0.5f does the top padding
    DrawDescBackground(
        pos,
        ImVec2((fit_background ? line : w) + ImGui::GetFontSize() * 2.0f,
               ImGui::GetFontSize() * (lines.size() + 1)),
        TextColor::NONE, m_border == TextColor::NONE ? border_col : m_border);
    m_cursor.y += ImGui::GetFontSize() * 0.5f;

    for (Desc &l : lines) {
      l.RenderLine(pos + m_cursor);
      m_cursor.y += ImGui::GetFontSize();
    }

    return m_cursor.y;
  overflow:
    DrawDescBackground(
        pos, ImVec2(w + ImGui::GetFontSize() * 2.0f, ImGui::GetFontSize()));
    Text("<->", TextColor::RED).Render(pos + m_cursor);
    return m_cursor.y;
  }

  void Append(Text t) { m_text.push_back(std::move(t)); }

private:
  std::vector<Text> m_text;
  ImVec2 m_cursor;

  TextColor m_border;

  void ResetCursor() { m_cursor = {0.0f, 0.0f}; }
};
