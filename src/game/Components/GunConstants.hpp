#pragma once

#include <chrono>
#include <cstddef>
#include "TimeConstants.hpp"

namespace rtype::game {

/**
 * @brief The most missiles a ship fires per second, while its player holds
 * shoot.
 *
 * Provisional, until the game is tuned.
 */
constexpr std::size_t SHOTS_PER_SECOND = 3;

static_assert(engine::SIMULATION_TICKS_PER_SECOND % SHOTS_PER_SECOND == 0,
              "the fire rate must divide the tick rate: a ship fires every "
              "whole number of ticks");

/**
 * @brief Simulation ticks between two missiles of one ship: 20 at 60 ticks per
 * second.
 */
constexpr std::size_t SHOT_INTERVAL_TICKS =
    engine::SIMULATION_TICKS_PER_SECOND / SHOTS_PER_SECOND;

/**
 * @brief Time between two missiles of one Bydo, in simulation time; a new Bydo
 * also waits that long before its first missile.
 *
 * Provisional, until the game is tuned. Each Bydo counts it from its own
 * appearance, so Bydos that appeared a multiple of it apart fire on the same
 * tick: the Bydos of a wave that brings one every 1.5 s fire together.
 */
constexpr engine::Duration ENEMY_SHOT_INTERVAL =
    std::chrono::milliseconds(1500);

/**
 * @brief ENEMY_SHOT_INTERVAL in whole simulation ticks, rounded down: 90 ticks
 * at 60 per second.
 */
constexpr std::size_t ENEMY_SHOT_INTERVAL_TICKS =
    engine::wholeTicksIn(ENEMY_SHOT_INTERVAL);

static_assert(ENEMY_SHOT_INTERVAL_TICKS >= 1,
              "ENEMY_SHOT_INTERVAL must last at least one simulation tick");

}  // namespace rtype::game
