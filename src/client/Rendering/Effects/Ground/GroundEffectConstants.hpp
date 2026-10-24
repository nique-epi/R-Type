#pragma once

#include <array>
#include <cstddef>
#include <numbers>

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

}  // namespace rtype::client
