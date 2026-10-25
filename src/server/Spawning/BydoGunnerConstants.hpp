#pragma once

#include <chrono>
#include <cstddef>
#include <string_view>
#include "TimeConstants.hpp"

namespace rtype::server {

/**
 * @brief Time between two missiles of one Bydo, in simulation time; a new Bydo
 * also waits that long before its first missile.
 *
 * Provisional, until the game is tuned. It differs from BYDO_SPAWN_INTERVAL, so
 * the Bydos on screen fire in turn, a group every half second, instead of all
 * on the same tick.
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

/**
 * @brief Module name the gunner writes its log lines under.
 */
constexpr std::string_view BYDO_GUNNER_LOGGER_NAME = "BydoGunner";

}  // namespace rtype::server
