#pragma once

#include <chrono>
#include <cstddef>
#include <string_view>

/** @brief The backgrounds the repository ships, and a biome none of them is
 * named after. */
constexpr std::string_view SHIPPED_SKY = "space";
constexpr std::string_view SHIPPED_PIT = "fosse";
constexpr std::string_view BIOME_WITHOUT_A_BACKGROUND = "nowhere";

/** @brief A folder that does not exist: its catalog holds no background. */
constexpr std::string_view FOLDER_WITHOUT_BACKGROUNDS = "no-such-assets";

/** @brief The layers of the sky and of the Pit the tests follow. */
constexpr std::size_t FAR_STARS_OF_THE_SKY = 0;
constexpr std::size_t PLANET_OF_THE_SKY = 1;
constexpr std::size_t FAR_WALLS_OF_THE_PIT = 0;
constexpr std::size_t NEAR_WALLS_OF_THE_PIT = 2;

/** @brief Twenty frames of 50 ms make one second, in which the planet
 * drifts 1.8 pixels to the left. */
constexpr std::chrono::milliseconds ONE_FRAME_OF_SKY{50};
constexpr int FRAMES_OF_ONE_SECOND = 20;
constexpr float PLANET_AFTER_ONE_SECOND_OF_SKY = -1.8F;
constexpr float ROUNDING_OF_THE_SKY = 0.001F;

/** @brief A stall of ten seconds moves the sky by its longest step, 50 ms:
 * the planet by 0.09 pixel. */
constexpr std::chrono::seconds A_STALL{10};
constexpr float PLANET_AFTER_A_STALL = -0.09F;

/** @brief Scrolls of a level, in logical units, four to a background pixel.
 * The entrance of the Pit begins on the column 592 when it begins: 400 units
 * in, it stands on 492 and still shows; 2600 units in, on -58, out of
 * sight with its fringe. */
constexpr float WHERE_THE_PIT_BEGAN = 1000.0F;
constexpr float FOUR_HUNDRED_UNITS = 400.0F;
constexpr float TWO_THOUSAND_SIX_HUNDRED_UNITS = 2600.0F;
constexpr int ENTRANCE_FOUR_HUNDRED_UNITS_IN = 380;
constexpr int RIGHT_EDGE_OF_THE_SKY = 480;

/** @brief A level 100 units in that scrolls 120 units per second: one second
 * later it is 220 units in, 55 background pixels, and the far walls of the
 * Pit, which follow a quarter of the scroll, lie 13.75 pixels to the left. */
constexpr float A_HUNDRED_UNITS = 100.0F;
constexpr float A_HUNDRED_AND_TWENTY_UNITS_PER_SECOND = 120.0F;
constexpr float FAR_WALLS_ONE_SECOND_LATER = -13.75F;

/** @brief At forty times their speed, the far stars leave a streak of 3
 * pixels; a speed factor below zero counts as zero. */
constexpr float FORTY_TIMES_THE_SPEED = 40.0F;
constexpr int STREAK_OF_THE_FAR_STARS = 3;
constexpr float A_SPEED_FACTOR_BELOW_ZERO = -1.0F;
