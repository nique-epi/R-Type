#pragma once

#include <cstddef>
#include <string_view>
#include "TimeConstants.hpp"

namespace rtype::server {

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
 * @brief Module name the launcher writes its log lines under.
 */
constexpr std::string_view MISSILE_LAUNCHER_LOGGER_NAME = "MissileLauncher";

}  // namespace rtype::server
