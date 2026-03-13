#include "Game.hpp"
#include "Text.hpp"
#include <raylib.h>

int main() {
  Desc d(Text("BLUE", BLUE), Text("RED", RED));
  InitWindow(1920, 1080, "upgrader");

  Vector2 pos(1920.0f / 2.0f, 1080.0f / 2.0f);

  while (!WindowShouldClose()) {
    BeginDrawing();

    DrawRectanglePro(Rectangle(1920.0f / 2.0f, 1080.0f / 2.0f, 150.0f, 24.0f),
                     Vector2(75.0f, 0.0f), 0.0f, GREEN);
    d.Render(pos, 150.0f);

    EndDrawing();
  }
  CloseWindow();
}
