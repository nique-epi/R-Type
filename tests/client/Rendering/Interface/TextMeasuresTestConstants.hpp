#pragma once

#include <string_view>
#include "TextMetrics.hpp"

namespace rtype::client::testing {

/** @brief The three text sizes: as the fonts are drawn, half again as large,
 * twice as large, in half pixels per font pixel. */
constexpr TextMetrics NORMAL_TEXT{.halfPixelsPerFontPixel = 2};
constexpr TextMetrics LARGE_TEXT{.halfPixelsPerFontPixel = 3};
constexpr TextMetrics LARGEST_TEXT{.halfPixelsPerFontPixel = 4};

/** @brief The height of a line at each size: the 8 pixels the fonts are
 * designed for, 12, then 16. */
constexpr int NORMAL_LINE_HEIGHT = 8;
constexpr int LARGE_LINE_HEIGHT = 12;
constexpr int LARGEST_LINE_HEIGHT = 16;

/** @brief A word of five characters in the regular font, whose advance is one
 * line: 40, 60 and 80 pixels wide. */
constexpr std::string_view FIVE_LETTERS = "READY";
constexpr int FIVE_LETTERS_AT_LARGEST = 80;
/** @brief The same count of letters written in UTF-8 with an accent, which
 * takes two bytes: the width counts letters, not bytes. */
constexpr std::string_view FIVE_LETTERS_WITH_AN_ACCENT = "PRÊTS";

/** @brief A line 7 pixels wide as drawn, the widest Silkscreen letter: half
 * again as large it covers 10.5 pixels, rounded up to 11. */
constexpr int WIDEST_SMALL_LETTER = 7;
constexpr int WIDEST_SMALL_LETTER_AT_LARGE = 11;

/** @brief The scale of the WARNING banner, four times the font, in half
 * pixels: eight, whatever the size the player chose. */
constexpr int WARNING_SCALE = 4;
constexpr int WARNING_HALF_PIXELS = 8;
constexpr int PLAIN_SCALE = 1;

}  // namespace rtype::client::testing
