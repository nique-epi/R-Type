#pragma once

#include <array>

/** @brief A biome of one map 4 pixels wide, in pixels of its map: its floor
 * at rows 200, 210, none and 230, its ceiling at row 40 on its first column. */
constexpr std::array<int, 4> FLOOR_ROWS_OF_FOUR_COLUMNS{200, 210, -1, 230};
constexpr std::array<int, 4> CEILING_ROWS_OF_FOUR_COLUMNS{40, 41, 42, 43};

/** @brief Scrolls since the biome began, in logical units: when it begins its
 * first map stands at the right edge of the playfield, 1920 units; once the
 * level scrolled those 1920 units it stands at the left edge, and 16 units
 * (4 pixels, the width of the map) later the map has come back. */
constexpr float JUST_BEGUN = 0.0F;
constexpr float SCROLLED_A_SCREEN = 1920.0F;
constexpr float SCROLLED_A_SCREEN_AND_A_MAP = 1936.0F;

/** @brief Columns of the playfield, in logical units: the left edge, the
 * second and the third pixel, and the right edge. */
constexpr float LEFT_EDGE = 0.0F;
constexpr float SECOND_PIXEL = 4.0F;
constexpr float THIRD_PIXEL = 8.0F;
constexpr float RIGHT_EDGE = 1920.0F;

/** @brief The rows read there, in logical units: 4 a pixel. */
constexpr float FIRST_FLOOR_ROW = 800.0F;
constexpr float SECOND_FLOOR_ROW = 840.0F;
constexpr float FIRST_CEILING_ROW = 160.0F;
