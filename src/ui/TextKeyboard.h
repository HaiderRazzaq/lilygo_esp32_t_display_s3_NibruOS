#pragma once
#include "hardware/DisplayManager.h"
#include <cstring>

namespace TextKeyboard {
enum class Action : uint8_t { None, Character, Shift, Symbols, Delete, Space, Cancel, Accept };
struct Key {
  Action action;
  char character;
  Key(Action a = Action::None, char c = 0) : action(a), character(c) {}
};
inline char rowChar(unsigned row, unsigned index, bool caps, bool symbols) {
  const char* text = row == 0 ? "1234567890" : row == 1 ? "QWERTYUIOP" : row == 2 ? "ASDFGHJKL" : "ZXCVBNM";
  if (symbols && row) text = row == 1 ? "@#$%&*-+_!" : row == 2 ? "()[]{}=;:" : "?.,/\\~`";
  if (index >= std::strlen(text)) return 0;
  char c = text[index];
  return !caps && !symbols && c >= 'A' && c <= 'Z' ? c + ('a' - 'A') : c;
}
inline Key hit(int x, int y, bool caps = true, bool symbols = false) {
  if (x < 0 || x >= 320) return {};
  if (y >= 52 && y < 110) {
    const unsigned row = (y - 52) / 20;
    if ((y - 52) % 20 >= 18) return {};
    const int left = row == 2 ? 8 : 2, width = row == 2 ? 34 : 32;
    if (x < left || (x - left) % width >= width - 2) return {};
    const char c = rowChar(row, (x - left) / width, caps, symbols);
    return c ? Key(Action::Character, c) : Key();
  }
  if (y >= 112 && y < 132) {
    if (x >= 2 && x < 42) return {Action::Shift};
    if (x >= 265 && x < 318) return {Action::Delete};
    if (x < 45 || x >= 262 || (x - 45) % 31 >= 29) return {};
    return {Action::Character, rowChar(3, (x - 45) / 31, caps, symbols)};
  }
  if (y >= 136 && y < 168) {
    if (x >= 2 && x < 54) return {Action::Symbols};
    if (x >= 57 && x < 130) return {Action::Space, ' '};
    if (x >= 133 && x < 218) return {Action::Cancel};
    if (x >= 221 && x < 318) return {Action::Accept};
  }
  return {};
}
inline void button(DisplayManager& screen, int x, int y, int width, int height, const char* label, uint16_t color) {
  screen.gfx().fillRoundRect(x, y, width, height, 3, Colors::Panel);
  screen.gfx().drawRoundRect(x, y, width, height, 3, color);
  const int textWidth = static_cast<int>(std::strlen(label)) * 6;
  screen.label(x + (width - textWidth) / 2, y + (height - 8) / 2, label, color, 1, Colors::Panel);
}
inline void draw(DisplayManager& screen, bool caps, bool symbols, const char* accept, bool base32 = false) {
  screen.gfx().fillRect(0, 51, 320, 119, Colors::Bg);
  for (unsigned row = 0; row < 4; ++row) {
    const unsigned count = row < 2 ? 10 : row == 2 ? 9 : 7;
    const int left = row < 2 ? 2 : row == 2 ? 8 : 45;
    const int width = row < 2 ? 32 : row == 2 ? 34 : 31;
    for (unsigned i = 0; i < count; ++i) {
      const char c = rowChar(row, i, caps, symbols);
      const char label[] = {c, 0};
      const bool valid = !base32 || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '2' && c <= '7');
      button(screen, left + i * width, 52 + row * 20, width - 2, row == 3 ? 20 : 18, label, valid ? Colors::White : Colors::Muted);
    }
  }
  button(screen, 2, 112, 40, 20, caps ? "SHIFT" : "shift", Colors::Cyan);
  button(screen, 265, 112, 53, 20, "DEL", Colors::Red);
  button(screen, 2, 136, 52, 32, symbols ? "ABC" : "#+=", Colors::Cyan);
  button(screen, 57, 136, 73, 32, "SPACE", base32 ? Colors::Muted : Colors::White);
  button(screen, 133, 136, 85, 32, "CANCEL", Colors::Amber);
  button(screen, 221, 136, 97, 32, accept, Colors::Cyan);
}
}
