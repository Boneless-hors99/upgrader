#pragma once
#include <memory>
#include <raylib.h>
#include <raymath.h>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

class Text {
public:
  Text(std::string_view str, Color col = WHITE, Color backcol = Color(0.0f))
      : m_str(str), m_col(col), m_backcol(backcol) {}

  std::string str();
  const char *chr();

  Vector2 msr();
  static Vector2 msr(std::string str);

  bool fits(float w);

  std::pair<Text, Text> split(float w);

  void Render(Vector2 pos);

private:
  std::string m_str;
  Color m_col;
  Color m_backcol;
};

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

  void RenderLine(Vector2 pos) {
    ResetCursor();
    m_cursor.x -= msr() / 2.0f;
    for (const auto &t : m_text) {
      t->Render(pos + m_cursor);
      m_cursor.x += t->msr().x;
    }
  }

  float Render(Vector2 pos, float w) {
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

    for (Desc &l : lines) {
      l.RenderLine(pos + m_cursor);
      m_cursor.y += 24.0f;
    }

    return m_cursor.y;
  overflow:
    Text("<->", RED).Render(pos + m_cursor);
    return -1;
  }

private:
  void Append(std::unique_ptr<Text> t) { m_text.push_back(std::move(t)); }

  std::vector<std::unique_ptr<Text>> m_text;
  Vector2 m_cursor;

  void ResetCursor() { m_cursor = {0.0f, 0.0f}; }
};
