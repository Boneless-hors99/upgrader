#pragma once
#include <memory>
#include <raylib.h>
#include <raymath.h>
#include <string>
#include <string_view>
#include <vector>

class Text {
public:
  Text(std::string_view str, Color col = WHITE, Color backcol = Color(0.0f))
      : m_str(str), m_col(col), m_backcol(backcol) {}

  std::string str();
  const char *chr();

  Vector2 msr();
  static Vector2 msr(std::string str) {
    return MeasureTextEx(GetFontDefault(), str.c_str(), 24.0f, 1.0f);
  }
  std::pair<Text, Text> split(float w);

  void Render(Vector2 pos);

private:
  std::string m_str;
  Color m_col;
  Color m_backcol;
};

template <typename... Args> class Desc {
  static_assert((std::is_base_of_v<Text, Args> && ...),
                "All arguments must derive from Text");

public:
  Desc(Args &&...args) {
    (Append(std::make_unique<Args>(std::forward<Args>(args))), ...);
  }

  float msr();
  float Render(Vector2 pos, float w) {
    Vector2 curs = Vector2(0.0f, 0.0f);
    for (const auto &t : m_text) {
      bool text_split = false;
      Text ctxt = *t.get();
      while (!text_split) {

        auto pair = ctxt.split(w - curs.x);

        // Render first text at cursor & move cursor
        if (!pair.first.str().empty()) {
          Vector2 tpos = pos + curs;
          pair.first.Render(tpos);
          curs.x += pair.first.msr().x;
        }

        if (pair.second.str().empty()) {
          text_split = true;
        } else {
          ctxt = pair.second;
          curs.y += pair.second.msr().y;
          curs.x = 0;
        }
      }
    }
    return curs.y;
  }

private:
  void Append(std::unique_ptr<Text> t) { m_text.push_back(std::move(t)); }
  std::vector<std::unique_ptr<Text>> m_text;
};
