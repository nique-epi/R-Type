#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include "TimeConstants.hpp"

namespace rtype::game {

/** @brief Each player past the first adds BOSS_HEALTH_PER_EXTRA_PLAYER
 * percent to the health of a boss: 240 points for one player, 420, 600 and
 * 780 for two, three and four. A team of four fires four times as much as
 * one pilot and takes 325 % of the health: the fight lasts 81 % of what it
 * lasts alone. */
constexpr int BOSS_HEALTH_PER_EXTRA_PLAYER = 75;

/** @brief At the most threat, a boss arrives with this percent more health; a
 * smaller threat gets its share. A pilot alone at the strongest weapon fires
 * five times its gun and meets 250 % of the health: the fight lasts half of
 * what it lasts with the gun. */
constexpr int BOSS_HEALTH_AT_THE_MOST_THREAT = 150;

/** @brief How long the players are warned of a boss before anything steers
 * it: 600 ms of silence, the 3200 ms WARNING, and 600 ms of margin. */
constexpr engine::Duration BOSS_ARRIVAL_WARNING =
    std::chrono::milliseconds(4400);
constexpr auto BOSS_ARRIVAL_WARNING_MILLISECONDS = static_cast<std::uint32_t>(
    std::chrono::duration_cast<std::chrono::milliseconds>(BOSS_ARRIVAL_WARNING)
        .count());
constexpr std::size_t BOSS_ARRIVAL_WARNING_TICKS =
    engine::wholeTicksIn(BOSS_ARRIVAL_WARNING);

}  // namespace rtype::game
