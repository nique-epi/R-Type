#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include "TimeConstants.hpp"

namespace rtype::server {

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

/**
 * @brief The score at which a player wins its first extra life, and the score
 * it wins one more at each time after: 30000, 90000, 150000 and so on, as
 * arcade shooters give a life at fixed steps of the score. A player never
 * holds more than identifiers::HIGHEST_LIVES.
 *
 * Provisional: gameplay will tune them against the scores of whole games.
 */
constexpr std::uint32_t FIRST_EXTRA_LIFE_SCORE = 30000;
constexpr std::uint32_t EXTRA_LIFE_SCORE_STEP = 60000;

}  // namespace rtype::server
