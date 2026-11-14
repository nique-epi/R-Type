#pragma once

#include <array>
#include <cstddef>

/** @brief One map of the Pit, 1440 pixels wide, and two maps of 300 pixels
 * laid end to end, 600 pixels in all. */
constexpr std::array ONE_MAP_OF_THE_PIT{1440};
constexpr std::array TWO_SHORT_MAPS{300, 300};
constexpr std::array A_MAP_WITHOUT_WIDTH{0};

/** @brief The first and the second map of a list. */
constexpr std::size_t FIRST_MAP = 0;
constexpr std::size_t SECOND_MAP = 1;

/** @brief Columns the first map starts on: the right edge of the sky (480),
 * 100 pixels left of it, 250 pixels left of the left edge of the sky, and
 * both short maps (600 pixels) left of it. */
constexpr int ON_THE_RIGHT_EDGE = 480;
constexpr int A_HUNDRED_PIXELS_INTO_THE_SKY = 380;
constexpr int TWO_HUNDRED_FIFTY_LEFT_OF_THE_SKY_EDGE = -250;
constexpr int BOTH_SHORT_MAPS_LEFT_OF_THE_SKY = -600;

/** @brief Columns of the sky, in background pixels: 100 pixels left of its
 * right edge (480), 250 left of its left edge, then the next two maps of
 * 300, and its left edge followed by the next map. */
constexpr int A_HUNDRED_PIXELS_LEFT_OF_THE_RIGHT_EDGE = 380;
constexpr int TWO_HUNDRED_FIFTY_LEFT_OF_THE_SKY = -250;
constexpr int SECOND_SHORT_MAP_AFTER_THE_SEAM = 50;
constexpr int FIRST_SHORT_MAP_AGAIN = 350;
constexpr int LEFT_EDGE_OF_THE_SKY = 0;
constexpr int ONE_SHORT_MAP_INTO_THE_SKY = 300;
