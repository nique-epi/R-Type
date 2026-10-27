#pragma once

#include <array>
#include <chrono>
#include <cstddef>
#include <numbers>
#include "BackgroundConstants.hpp"
#include "CollisionConstants.hpp"
#include "FireColorConstants.hpp"
#include "ShipControlConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/** @brief A particle that shrinks is two by two pixels until it lived
 * PARTICLE_SHRINK_SHARE of its life, then one. */
constexpr float PARTICLE_SHRINK_SHARE = 0.4F;

/** @brief Share of its speed a particle keeps over each
 * SIMULATION_TICK_DURATION: it loses 10 % a frame. */
constexpr float PARTICLE_SPEED_KEPT_PER_TICK = 0.9F;

/** @brief Most particles alive at once: a burst that would pass it keeps its
 * first particles and drops the others. */
constexpr std::size_t PARTICLE_MAXIMUM = 192;

/** @brief A shockwave lights one pixel in two on the last
 * SHOCKWAVE_DOTTED_SHARE of its life. */
constexpr float SHOCKWAVE_DOTTED_SHARE = 1.0F / 3.0F;

/**
 * @brief The head of a speed line moves SPEED_LINE_HEAD_PACE times the pace
 * of its life, so it reaches the ship at 62.5 % of it; its tail runs
 * SPEED_LINE_TAIL_DELAY behind, at the same pace, and reaches the ship at the
 * end of the life.
 */
constexpr float SPEED_LINE_HEAD_PACE = 1.6F;
constexpr float SPEED_LINE_TAIL_DELAY = 0.6F;

/** @brief Directions a burst aims at, in radians from the right, the rows of
 * the canvas growing downwards, and the cone of a burst all around. */
constexpr float TOWARDS_RIGHT = 0.0F;
constexpr float TOWARDS_LEFT = std::numbers::pi_v<float>;
constexpr float ALL_AROUND = 2.0F * std::numbers::pi_v<float>;

/** @brief Rows of the canvas, away from the center of a ship arriving, that
 * its four speed lines run on. */
constexpr std::array SPEED_LINE_ROWS{-6, -2, 2, 6};

/** @brief Pixels from the center of a ship to its engine: half its width. */
constexpr int SHIP_HALF_WIDTH_PIXELS =
    static_cast<int>(game::SHIP_WIDTH * engine::HALF / BACKGROUND_PIXEL_SIZE);

/**
 * @brief The flame behind the engine of a ship: 1 pixel when out,
 * FLAME_LONGEST at full thrust, one more every other FLAME_FLICKER_STEP. Its
 * colors from the engine out, its last quarter one row thin.
 */
constexpr int FLAME_LONGEST = 8;
constexpr engine::Duration FLAME_FLICKER_STEP = std::chrono::milliseconds(50);
constexpr std::array FLAME_COLORS{FIRE_YELLOW, FIRE_GOLD, FIRE_GOLD, FIRE_RED};
constexpr float FLAME_TIP_SHARE = 0.75F;

/**
 * @brief The flight lines behind a ship pushed forward for FLIGHT_LINES_AFTER
 * (full speed at 350 ms, then 300 ms) or dashing: one on each row of
 * SPEED_LINE_ROWS, FLIGHT_LINE_LENGTHS long, FLIGHT_LINE_GAP pixels behind the
 * engine, scrolling FLIGHT_LINE_PACE pixels per second over FLIGHT_LINE_PERIOD
 * pixels, each FLIGHT_LINE_PHASES ahead.
 */
constexpr engine::Duration FLIGHT_LINES_AFTER = std::chrono::milliseconds(650);
constexpr std::array FLIGHT_LINE_LENGTHS{8, 13, 9, 14};
constexpr std::array FLIGHT_LINE_PHASES{0, 7, 14, 21};
constexpr int FLIGHT_LINE_GAP = 6;
constexpr float FLIGHT_LINE_PACE = 180.0F;
constexpr int FLIGHT_LINE_PERIOD = 24;

}  // namespace rtype::client
