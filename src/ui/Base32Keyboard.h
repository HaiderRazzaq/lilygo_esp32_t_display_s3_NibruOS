#pragma once
// Compatibility wrapper. Shared QWERTY implementation lives in TextKeyboard.h.
#include "ui/TextKeyboard.h"
namespace Base32Keyboard {
inline char keyAt(int x, int y) { return TextKeyboard::hit(x, y).character; }
inline void draw(DisplayManager& screen) { TextKeyboard::draw(screen, true, false, "SAVE", true); }
}
