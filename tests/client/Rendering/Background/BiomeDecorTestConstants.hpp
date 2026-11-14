#pragma once

#include <chrono>
#include <cstddef>
#include <string_view>
#include "TimeConstants.hpp"

/** @brief The biome the client has a decor for, its rank among the biomes,
 * and a name no biome has. */
constexpr std::string_view KNOWN_BIOME = "fosse";
constexpr std::size_t RANK_OF_THE_KNOWN_BIOME = 0;
constexpr std::string_view UNKNOWN_BIOME = "nowhere";

/** @brief A file no assets folder holds. */
constexpr std::string_view FILE_NOWHERE = "/nowhere/no_biome.ldtk";

/** @brief A level scrolled 100 logical units that scrolls 120 units per
 * second, where it is one second later, and a level whose scroll is
 * paused. */
constexpr float SHOWN_SCROLL_POSITION = 100.0F;
constexpr float SHOWN_SCROLL_SPEED = 120.0F;
constexpr float SCROLL_POSITION_ONE_SECOND_LATER = 220.0F;
constexpr float PAUSED_SCROLL = 0.0F;
constexpr rtype::engine::Duration ONE_SECOND_OF_DECOR = std::chrono::seconds(1);
constexpr rtype::engine::Duration ONE_SECOND_BACKWARDS_OF_DECOR =
    std::chrono::seconds(-1);

/** @brief A level of the known biome scrolled 102 logical units since the
 * biome began, off the grid of its far and middle planes: the far plane
 * entered 6.375 pixels, the middle one 12.75, the near one 25.5. */
constexpr float SCROLL_OFF_THE_GRID = 102.0F;
constexpr float BIOME_BEGAN_AT_THE_START = 0.0F;
constexpr float FAR_PLANE_SHIFT_OFF_THE_GRID = -0.375F;
constexpr float MIDDLE_PLANE_SHIFT_OFF_THE_GRID = -0.75F;

/** @brief A level of the Pit that has not scrolled since it began, and one
 * that scrolled 400 logical units: its near plane entered 100 pixels. The
 * first near tile of the Pit lies on column 112 of its first map, so the
 * seam lies 112 columns right of the right edge of the sky, 592, then 100
 * columns further left, 492. */
constexpr float SCROLL_AT_THE_START_OF_THE_PIT = 0.0F;
constexpr float SCROLL_OF_A_HUNDRED_NEAR_PIXELS = 400.0F;
constexpr int SEAM_AT_THE_START_OF_THE_PIT = 592;
constexpr int SEAM_A_HUNDRED_PIXELS_IN = 492;

/** @brief A level of the Pit that scrolled 402 logical units: its near plane
 * entered 100.5 pixels, and the seam stays on the whole column of 100, 492. */
constexpr float SCROLL_OF_A_HUNDRED_AND_A_HALF_NEAR_PIXELS = 402.0F;
