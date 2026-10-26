#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include "TimeConstants.hpp"

namespace rtype::server {

/**
 * @brief Lives a player starts each game with, the ship in play included: its
 * ship can be destroyed this many times before the player only watches.
 *
 * Provisional: gameplay will tune it.
 */
constexpr std::uint8_t STARTING_LIVES = 3;

/**
 * @brief How long a player whose ship was destroyed, with a life left, has no
 * ship before a new one appears at its start position, in simulation time.
 *
 * Provisional: gameplay will tune it.
 */
constexpr engine::Duration RESPAWN_DELAY = std::chrono::seconds(1);

/**
 * @brief RESPAWN_DELAY in whole simulation ticks, rounded down: 60 ticks at 60
 * per second.
 */
constexpr std::size_t RESPAWN_DELAY_TICKS = engine::wholeTicksIn(RESPAWN_DELAY);

static_assert(RESPAWN_DELAY_TICKS >= 1,
              "RESPAWN_DELAY must last at least one simulation tick");

/**
 * @brief How long a ship that came back after a hit cannot be hit, in
 * simulation time: an enemy it touches meanwhile costs nothing and is not
 * destroyed.
 *
 * Provisional: gameplay will tune it.
 */
constexpr engine::Duration INVULNERABILITY_DURATION = std::chrono::seconds(2);

/**
 * @brief INVULNERABILITY_DURATION in whole simulation ticks, rounded down: 120
 * ticks at 60 per second.
 */
constexpr std::size_t INVULNERABILITY_TICKS =
    engine::wholeTicksIn(INVULNERABILITY_DURATION);

}  // namespace rtype::server
