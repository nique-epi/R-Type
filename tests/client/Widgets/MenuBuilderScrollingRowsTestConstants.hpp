#pragma once

#include <SFML/System/Vector2.hpp>
#include <array>
#include <cstddef>
#include "TextMetrics.hpp"

namespace rtype::client::testing {

/** @brief The labels of a panel of nine rows, one more than its eight. */
constexpr std::array<const char*, 9> NINE_ROW_LABELS{"ROW 1", "ROW 2", "ROW 3",
                                                     "ROW 4", "ROW 5", "ROW 6",
                                                     "ROW 7", "ROW 8", "ROW 9"};

/** @brief Where the label of the first and of the eighth shown row is
 * written: the tabs at 24 + 8 + 8 + 1 + 9 = 50, the panel 16 lower at 66,
 * the first row 4 inside it at 70, its label 4 lower and 8 right of the
 * row's left at 28; each row 18 below the one above, the eighth at 196. */
constexpr sf::Vector2i FIRST_SHOWN_ROW_LABEL(36, 74);
constexpr sf::Vector2i LAST_SHOWN_ROW_LABEL(36, 200);

/** @brief Down presses that move the selection from the first row to the
 * ninth, and the order of the second row among the buttons. */
constexpr std::size_t PRESSES_TO_THE_NINTH_ROW = 8;
constexpr std::size_t SECOND_ROW = 1;

/** @brief One notch of the wheel toward the player, which scrolls down. */
constexpr float ONE_NOTCH_DOWN = -1.0F;

/** @brief The largest text size, and where its fourth and last shown row
 * writes its label: tabs at 24 + 16 + 8 + 1 + 9 = 58, 24 tall, the panel at
 * 82, the first row 4 inside at 86, rows 24 tall and 26 apart, the fourth at
 * 164, its label centered 4 lower. */
constexpr TextMetrics LARGEST_TEXT{.halfPixelsPerFontPixel = 4};
constexpr sf::Vector2i FOURTH_ROW_LABEL_AT_LARGEST(36, 168);

/** @brief The description of every row, 36 letters: 576 pixels at the
 * largest size, where 336 are left right of BACK (88 wide from 24, then 8),
 * so it takes two lines of at most 21 letters, the second 16 + 2 lower. */
constexpr const char* LONG_DESCRIPTION = "> STRONGER COLORS, PLATES UNDER TEXT";
constexpr const char* FIRST_DESCRIPTION_LINE = "> STRONGER COLORS,";
constexpr const char* SECOND_DESCRIPTION_LINE = "PLATES UNDER TEXT";
constexpr sf::Vector2i FIRST_DESCRIPTION_LINE_AT_LARGEST(120, 210);
constexpr sf::Vector2i SECOND_DESCRIPTION_LINE_AT_LARGEST(120, 228);

/** @brief The illustration a page asks for, as large as the squadron of the
 * title, and where the label of the second of four bars of a page lies at
 * the largest size: two columns of (480 - 48 - 8) / 2 = 212, the second at
 * 24 + 212 + 8 = 244, its label 12 in; bars from 24 + 16 + 8 + 1 + 9 = 58,
 * 28 tall, the label centered 6 lower. */
constexpr sf::Vector2i SQUADRON_LIKE_AREA(168, 144);
constexpr sf::Vector2i SECOND_BAR_LABEL_AT_LARGEST(256, 64);

/** @brief A pixel inside the first shown row, left of its control. */
constexpr sf::Vector2i INSIDE_THE_FIRST_SHOWN_ROW(60, 80);

}  // namespace rtype::client::testing
