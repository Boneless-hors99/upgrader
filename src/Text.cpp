#include "Text.hpp"
#include <raylib.h>
#include <raymath.h>
#include <sstream>
#include <string>
#include <utility>

std::string Text::str() { return m_str; }
const char *Text::chr() { return m_str.c_str(); }

Vector2 Text::msr() { return msr(m_str); }

std::pair<Text, Text> Text::split(float w) {
  Vector2 size = msr();
  if (size.x <= w) {
    return {*this, Text("")};
  }
  std::istringstream t(m_str);
  std::string s, r, e;
  bool enuf = false;

  while (std::getline(t, s, ' ')) {
    if (!enuf && msr(r + s).x <= w) {
      r += s + ' ';
    } else {
      enuf = true;
      e += s + ' ';
    }
  }
  return {Text(r, m_col, m_backcol), Text(e, m_col, m_backcol)};
}

void Text::Render(Vector2 pos) {
  DrawTextPro(GetFontDefault(), chr(), pos, Vector2(msr().x / 2.0f, 0.0f), 0.0f,
              24.0f, 1.0f, m_col);
}

template <> float Desc<Text>::msr() {
  float sum{};
  for (const auto &t : m_text)
    sum += t->msr().x;
  return sum;
}
