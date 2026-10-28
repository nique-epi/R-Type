#pragma once

#include <array>
#include <cstddef>
#include "BackgroundColorConstants.hpp"
#include "IntroExhaustRow.hpp"

namespace rtype::client {

/** @brief A ship leaves INTRO_SMEAR_COPIES ghosts, INTRO_SMEAR_SPACING pixels
 * apart at a full smear, none under INTRO_SMEAR_THRESHOLD. */
constexpr std::size_t INTRO_SMEAR_COPIES = 3;
constexpr float INTRO_SMEAR_SPACING = 9.0F;
constexpr float INTRO_SMEAR_THRESHOLD = 0.05F;

/** @brief The flame of a ship: none under INTRO_EXHAUST_THRESHOLD, else
 * INTRO_EXHAUST_BASE + INTRO_EXHAUST_GROWTH times its length pixels on its
 * longest rows, in INTRO_FLAME_SHADES shades from white at the nozzle; its
 * rows under the top of the sprite. */
constexpr float INTRO_EXHAUST_THRESHOLD = 0.05F;
constexpr float INTRO_EXHAUST_BASE = 2.0F;
constexpr float INTRO_EXHAUST_GROWTH = 16.0F;
constexpr std::size_t INTRO_FLAME_SHADES = 4;
constexpr std::array INTRO_EXHAUST_ROWS{
    IntroExhaustRow{.row = 6, .share = 0.55F},
    IntroExhaustRow{.row = 7, .share = 1.0F},
    IntroExhaustRow{.row = 8, .share = 1.0F},
    IntroExhaustRow{.row = 9, .share = 0.55F}};

/** @brief The ball of a charge has INTRO_BALL_LAYERS layers, each one pixel
 * smaller; a beam has INTRO_BEAM_BANDS bands, from the dark shade at its edge
 * to white in its middle. */
constexpr std::size_t INTRO_BALL_LAYERS = 3;
constexpr std::size_t INTRO_BEAM_BANDS = 4;

/** @brief The smoke a curved missile leaves: one puff per color of
 * INTRO_SMOKE_RAMP, INTRO_SMOKE_SPACING of its flight apart, the
 * INTRO_LARGE_PUFFS nearest two pixels large. */
constexpr std::array INTRO_SMOKE_RAMP{STAR_WHITE, PALE_BLUE, PALE_BLUE,
                                      BLUE,       DEEP_BLUE, DEEP_BLUE};
constexpr float INTRO_SMOKE_SPACING = 0.06F;
constexpr std::size_t INTRO_LARGE_PUFFS = 3;
constexpr int INTRO_LARGE_PUFF = 2;
constexpr int INTRO_SMALL_PUFF = 1;

/** @brief The muzzle flash of a missile shows INTRO_MUZZLE_TIME from its
 * shot, INTRO_MUZZLE_RISE pixels above the muzzle: half its size, rounded
 * down. */
constexpr float INTRO_MUZZLE_TIME = 0.1F;
constexpr float INTRO_MUZZLE_RISE = 3.0F;

/**
 * @brief Speed lines: none under INTRO_SPEED_LINE_FLOOR times the speed of
 * the stars, INTRO_SPEED_LINE_COUNT at INTRO_SPEED_LINE_FLOOR +
 * INTRO_SPEED_LINE_RANGE and over, in the colors of INTRO_SPEED_LINE_COLORS
 * in turn. Line i lies on a row, length, speed and start drawn from the hash
 * of i times its seed: INTRO_LINE_SHORTEST plus up to INTRO_LINE_LENGTH_SPREAD
 * pixels long, INTRO_LINE_SLOWEST plus up to INTRO_LINE_SPEED_SPREAD pixels a
 * second, looping every INTRO_LINE_LOOP pixels from INTRO_LINE_LEAD left of
 * the screen.
 */
constexpr float INTRO_SPEED_LINE_FLOOR = 4.0F;
constexpr float INTRO_SPEED_LINE_RANGE = 8.0F;
constexpr float INTRO_SPEED_LINE_COUNT = 40.0F;
constexpr std::array INTRO_SPEED_LINE_COLORS{STAR_WHITE, PALE_BLUE, BLUE};
constexpr int INTRO_LINE_SHORTEST = 16;
constexpr float INTRO_LINE_LENGTH_SPREAD = 64.0F;
constexpr float INTRO_LINE_SLOWEST = 1100.0F;
constexpr float INTRO_LINE_SPEED_SPREAD = 900.0F;
constexpr float INTRO_LINE_LOOP = 800.0F;
constexpr float INTRO_LINE_LEAD = 160.0F;
constexpr double INTRO_LINE_ROW_SEED = 3.1;
constexpr double INTRO_LINE_LENGTH_SEED = 7.7;
constexpr double INTRO_LINE_SPEED_SEED = 1.9;
constexpr double INTRO_LINE_START_SEED = 5.3;

/** @brief The hash of the mockup: the fractional part of
 * sin(seed × INTRO_HASH_FREQUENCY + INTRO_HASH_PHASE) × INTRO_HASH_GAIN. */
constexpr double INTRO_HASH_FREQUENCY = 127.1;
constexpr double INTRO_HASH_PHASE = 311.7;
constexpr double INTRO_HASH_GAIN = 43758.5453;

}  // namespace rtype::client
