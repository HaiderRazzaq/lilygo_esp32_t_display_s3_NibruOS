#include "services/ArabicReshaper.h"
#include <cstring>

namespace ArabicReshaper {

namespace {

struct CharForms {
  uint16_t isolated;
  uint16_t final_form;
  uint16_t initial;
  uint16_t medial;
};

// Mapping table for Arabic base letters (0x0621..0x064A)
static const CharForms kArabicTable[] = {
  // 0x0621: ء Hamza
  { 0xFE80, 0xFE80, 0xFE80, 0xFE80 },
  // 0x0622: آ Alef with Madda
  { 0xFE81, 0xFE82, 0xFE81, 0xFE82 },
  // 0x0623: أ Alef with Hamza above
  { 0xFE83, 0xFE84, 0xFE83, 0xFE84 },
  // 0x0624: ؤ Waw with Hamza
  { 0xFE85, 0xFE86, 0xFE85, 0xFE86 },
  // 0x0625: إ Alef with Hamza below
  { 0xFE87, 0xFE88, 0xFE87, 0xFE88 },
  // 0x0626: ئ Yeh with Hamza
  { 0xFE89, 0xFE8A, 0xFE8B, 0xFE8C },
  // 0x0627: ا Alef
  { 0xFE8D, 0xFE8E, 0xFE8D, 0xFE8E },
  // 0x0628: ب Beh
  { 0xFE8F, 0xFE90, 0xFE91, 0xFE92 },
  // 0x0629: ة Teh Marbuta
  { 0xFE93, 0xFE94, 0xFE93, 0xFE94 },
  // 0x062A: ت Teh
  { 0xFE95, 0xFE96, 0xFE97, 0xFE98 },
  // 0x062B: ث Theh
  { 0xFE99, 0xFE9A, 0xFE9B, 0xFE9C },
  // 0x062C: ج Jeem
  { 0xFE9D, 0xFE9E, 0xFE9F, 0xFEA0 },
  // 0x062D: ح Hah
  { 0xFEA1, 0xFEA2, 0xFEA3, 0xFEA4 },
  // 0x062E: خ Khah
  { 0xFEA5, 0xFEA6, 0xFEA7, 0xFEA8 },
  // 0x062F: د Dal
  { 0xFEA9, 0xFEAA, 0xFEA9, 0xFEAA },
  // 0x0630: ذ Thal
  { 0xFEAB, 0xFEAC, 0xFEAB, 0xFEAC },
  // 0x0631: ر Reh
  { 0xFEAD, 0xFEAE, 0xFEAD, 0xFEAE },
  // 0x0632: ز Zain
  { 0xFEAF, 0xFEB0, 0xFEAF, 0xFEB0 },
  // 0x0633: س Seen
  { 0xFEB1, 0xFEB2, 0xFEB3, 0xFEB4 },
  // 0x0634: ش Sheen
  { 0xFEB5, 0xFEB6, 0xFEB7, 0xFEB8 },
  // 0x0635: ص Sad
  { 0xFEB9, 0xFEBA, 0xFEBB, 0xFEBC },
  // 0x0636: ض Dad
  { 0xFEBD, 0xFEBE, 0xFEBF, 0xFEC0 },
  // 0x0637: ط Tah
  { 0xFEC1, 0xFEC2, 0xFEC3, 0xFEC4 },
  // 0x0638: ظ Zah
  { 0xFEC5, 0xFEC6, 0xFEC7, 0xFEC8 },
  // 0x0639: ع Ain
  { 0xFEC9, 0xFECA, 0xFECB, 0xFECC },
  // 0x063A: غ Ghain
  { 0xFECD, 0xFECE, 0xFECF, 0xFED0 },
  // 0x063B..0x0640 unused/tatweel
  { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0x0640, 0x0640, 0x0640, 0x0640 },
  // 0x0641: ف Feh
  { 0xFED1, 0xFED2, 0xFED3, 0xFED4 },
  // 0x0642: ق Qaf
  { 0xFED5, 0xFED6, 0xFED7, 0xFED8 },
  // 0x0643: ك Kaf
  { 0xFED9, 0xFEDA, 0xFEDB, 0xFEDC },
  // 0x0644: ل Lam
  { 0xFEDD, 0xFEDE, 0xFEDF, 0xFEE0 },
  // 0x0645: م Meem
  { 0xFEE1, 0xFEE2, 0xFEE3, 0xFEE4 },
  // 0x0646: ن Noon
  { 0xFEE5, 0xFEE6, 0xFEE7, 0xFEE8 },
  // 0x0647: ه Heh
  { 0xFEE9, 0xFEEA, 0xFEEB, 0xFEEC },
  // 0x0648: و Waw
  { 0xFEED, 0xFEEE, 0xFEED, 0xFEEE },
  // 0x0649: ى Alef Maksura
  { 0xFEEF, 0xFEF0, 0xFEEF, 0xFEF0 },
  // 0x064A: ي Yeh
  { 0xFEF1, 0xFEF2, 0xFEF3, 0xFEF4 },
};

bool isArabicLetter(uint16_t c) {
  return (c >= 0x0621 && c <= 0x064A);
}

bool connectsAfter(uint16_t c) {
  // Letters that do not connect after:
  switch (c) {
    case 0x0621: // ء
    case 0x0622: // آ
    case 0x0623: // أ
    case 0x0624: // ؤ
    case 0x0625: // إ
    case 0x0627: // ا
    case 0x0629: // ة
    case 0x062F: // د
    case 0x0630: // ذ
    case 0x0631: // ر
    case 0x0632: // ز
    case 0x0648: // و
    case 0x0649: // ى
      return false;
    default:
      return isArabicLetter(c);
  }
}

} // namespace

bool hasArabic(const char* utf8Text) {
  if (!utf8Text) return false;
  const unsigned char* p = reinterpret_cast<const unsigned char*>(utf8Text);
  while (*p) {
    if (*p == 0xD8 || *p == 0xD9) return true;
    ++p;
  }
  return false;
}

size_t reshape(const char* utf8Text, uint16_t* outGlyphs, size_t maxGlyphs) {
  if (!utf8Text || !outGlyphs || maxGlyphs == 0) return 0;

  // 1. Decode UTF-8 string into raw Unicode code points
  uint16_t raw[128];
  size_t rawCount = 0;
  const unsigned char* p = reinterpret_cast<const unsigned char*>(utf8Text);

  while (*p && rawCount < 127) {
    if (*p < 0x80) {
      raw[rawCount++] = *p++;
    } else if ((*p == 0xD8 || *p == 0xD9) && *(p + 1)) {
      uint16_t c = static_cast<uint16_t>(((*p & 0x1F) << 6) | (*(p + 1) & 0x3F));
      p += 2;
      raw[rawCount++] = c;
    } else {
      // Skip unsupported multibyte or advance 1
      ++p;
    }
  }

  // 2. Perform cursive glyph shaping (Presentation Forms-B)
  uint16_t shaped[128];
  bool isArabicGlyph[128];
  size_t shapedCount = 0;

  size_t i = 0;
  while (i < rawCount && shapedCount < 127) {
    uint16_t c = raw[i];

    // Check Lam-Alef Ligatures
    if (c == 0x0644 && (i + 1 < rawCount)) {
      uint16_t nxt = raw[i + 1];
      const bool prevConn = (i > 0 && connectsAfter(raw[i - 1]));
      uint16_t lig = 0;
      if (nxt == 0x0627) lig = prevConn ? 0xFEFC : 0xFEFB; // لا
      else if (nxt == 0x0625) lig = prevConn ? 0xFEF8 : 0xFEF7; // لإ
      else if (nxt == 0x0623) lig = prevConn ? 0xFEF6 : 0xFEF5; // لأ
      else if (nxt == 0x0622) lig = prevConn ? 0xFEF4 : 0xFEF3; // لآ

      if (lig != 0) {
        shaped[shapedCount] = lig;
        isArabicGlyph[shapedCount] = true;
        shapedCount++;
        i += 2;
        continue;
      }
    }

    if (isArabicLetter(c)) {
      const bool prevConn = (i > 0 && connectsAfter(raw[i - 1]));
      const bool nextConn = (i + 1 < rawCount && isArabicLetter(raw[i + 1]) && raw[i + 1] != 0x0621);
      const uint8_t idx = static_cast<uint8_t>(c - 0x0621);
      const auto& forms = kArabicTable[idx];

      uint16_t g = forms.isolated;
      if (!prevConn && !nextConn) g = forms.isolated;
      else if (prevConn && !nextConn) g = forms.final_form;
      else if (!prevConn && nextConn) g = forms.initial;
      else g = forms.medial;

      shaped[shapedCount] = g;
      isArabicGlyph[shapedCount] = true;
      shapedCount++;
    } else {
      shaped[shapedCount] = c;
      isArabicGlyph[shapedCount] = false;
      shapedCount++;
    }
    ++i;
  }

  // 3. Bidirectional Reordering (RTL for Arabic, LTR for embedded Latin/digits)
  // Split into tokens: Arabic words are reversed, Latin words stay LTR, sentence reversed.
  // For standard compact system labels, reverse Arabic word glyphs in place:
  size_t start = 0;
  while (start < shapedCount) {
    if (isArabicGlyph[start]) {
      size_t end = start;
      while (end < shapedCount && isArabicGlyph[end]) ++end;
      // Reverse Arabic word in-place
      for (size_t a = start, b = end - 1; a < b; ++a, --b) {
        uint16_t tmp = shaped[a];
        shaped[a] = shaped[b];
        shaped[b] = tmp;
      }
      start = end;
    } else {
      ++start;
    }
  }

  // Now reverse the word order across spaces so sentences read right-to-left
  // Find words separated by space (0x0020) and reverse the sequence of words:
  uint16_t reordered[128];
  size_t outPos = 0;

  int endIdx = static_cast<int>(shapedCount) - 1;
  while (endIdx >= 0 && outPos < maxGlyphs) {
    int wordStart = endIdx;
    while (wordStart >= 0 && shaped[wordStart] != ' ') --wordStart;
    for (int k = wordStart + 1; k <= endIdx && outPos < maxGlyphs; ++k) {
      reordered[outPos++] = shaped[k];
    }
    if (wordStart >= 0 && outPos < maxGlyphs) {
      reordered[outPos++] = ' ';
    }
    endIdx = wordStart - 1;
  }

  for (size_t k = 0; k < outPos; ++k) {
    outGlyphs[k] = reordered[k];
  }
  return outPos;
}

} // namespace ArabicReshaper
