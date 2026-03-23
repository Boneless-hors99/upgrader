#pragma once
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#define WHITE ImVec4(1.0f, 1.0f, 1.0f, 1.0f)
#define LIGHT_GREY ImVec4(0.85f, 0.85f, 0.85f, 1.0f)
#define RED ImVec4(1.0f, 0.0f, 0.0f, 1.0f)
#define GREEN ImVec4(0.0f, 1.0f, 0.0f, 1.0f)
#define BLUE ImVec4(0.0f, 0.0f, 1.0f, 1.0f)

class Text {
public:
  Text(std::string_view str, ImVec4 col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
       ImVec4 backcol = ImVec4(0.0f, 0.0f, 0.0f, 0.0f))
      : m_str(str), m_col(col), m_backcol(backcol) {}

  std::string str();
  const char *chr();

  ImVec2 msr();
  static ImVec2 msr(std::string str);

  bool fits(float w);

  std::pair<Text, Text> split(float w);

  void Render(ImVec2 pos);

private:
  std::string m_str;
  ImVec4 m_col;
  ImVec4 m_backcol;
};

void DrawDescBackground(ImVec2 pos, ImVec2 size);

#define LINES std::vector<Desc>

class Desc {
public:
  Desc(const Desc &) = delete;
  Desc &operator=(const Desc &) = delete;

  Desc(Desc &&) = default;
  Desc &operator=(Desc &&) = default;

  template <typename... Args> Desc(Args &&...args) {
    static_assert((std::is_base_of_v<Text, Args> && ...),
                  "Args must derive from Text");
    (Append(std::make_unique<Args>(std::forward<Args>(args))), ...);
  }

  void Append(Text t) { Append(std::make_unique<Text>(t)); }

  // Returns total width of texts in description
  float msr() {
    float sum{};
    for (const auto &t : m_text)
      sum += t->msr().x;
    return sum;
  }

  static int AddText(LINES *lines, Text text, float maxwidth) {

    if (!text.fits(maxwidth)) {
      return -1;
    }

    int line;
    bool finished = false;
    while (!finished) {
      line = static_cast<int>(lines->size()) - 1;
      float occupied = (*lines)[line].msr();
      float space = maxwidth - occupied;

      auto splitted = text.split(space);

      if (splitted.first.str().empty())
        goto nextline;

      (*lines)[line].Append(splitted.first);

      if (splitted.second.str().empty()) {
        break;
      }

      text = splitted.second;

    nextline:
      lines->push_back(Desc(Text("")));
      line++;
    }
    return line;
  }

  void RenderLine(ImVec2 pos) {
    ResetCursor();
    float s = msr();
    m_cursor.x -= s / 2.0f;
    for (const auto &t : m_text) {
      ImVec2 tpos = pos;
      t->Render(tpos + m_cursor);
      m_cursor.x += t->msr().x;
    }
  }

  float Render(ImVec2 pos, float w) {
    ResetCursor();
    LINES lines;
    lines.emplace_back();
    int line = 0;

    // Turns this description into LINES with each being maximum w long
    for (const auto &t : m_text) {
      line = AddText(&lines, *t.get(), w);
      if (line < 0)
        goto overflow;
    }

    // *0.5f does the top and bottom padding
    DrawDescBackground(pos, ImVec2(w + ImGui::GetFontSize() * 2.0f,
                                   ImGui::GetFontSize() * (lines.size() + 1)));
    m_cursor.y += ImGui::GetFontSize() * 0.5f;

    for (Desc &l : lines) {
      l.RenderLine(pos + m_cursor);
      m_cursor.y += ImGui::GetFontSize();
    }

    return m_cursor.y;
  overflow:
    DrawDescBackground(
        pos, ImVec2(w + ImGui::GetFontSize() * 2.0f, ImGui::GetFontSize()));
    Text("<->", ImVec4(1.0f, 0.0f, 0.0f, 1.0f)).Render(pos + m_cursor);
    return m_cursor.y;
  }

private:
  void Append(std::unique_ptr<Text> t) { m_text.push_back(std::move(t)); }

  std::vector<std::unique_ptr<Text>> m_text;
  ImVec2 m_cursor;

  void ResetCursor() { m_cursor = {0.0f, 0.0f}; }
};
