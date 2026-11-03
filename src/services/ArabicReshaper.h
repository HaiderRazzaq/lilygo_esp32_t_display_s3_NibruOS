#pragma once
#include <stdint.h>
#include <stddef.h>

/**
 * @brief Embedded Arabic Glyph Reshaper & Bidirectional Reverser for NibruOS.
 * Connects Arabic cursive letters into Presentation Forms-B (Unicode 0xFE80..0xFEFC)
 * and applies visual RTL reordering for screen rasterization.
 */
namespace ArabicReshaper {

/**
 * @brief Checks if a UTF-8 string contains any Arabic characters.
 */
bool hasArabic(const char* utf8Text);

/**
 * @brief Reshapes and reverses an Arabic/bilingual UTF-8 string into display glyph codes.
 * @param utf8Text Input UTF-8 string.
 * @param outGlyphs Destination buffer for 16-bit glyph codes (ASCII or Presentation Forms-B).
 * @param maxGlyphs Maximum glyphs capacity in outGlyphs buffer.
 * @return Number of glyphs written.
 */
size_t reshape(const char* utf8Text, uint16_t* outGlyphs, size_t maxGlyphs);

} // namespace ArabicReshaper
