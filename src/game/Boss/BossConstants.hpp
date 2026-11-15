#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include "TimeConstants.hpp"

namespace rtype::game {

/** @brief Each player past the first adds BOSS_HEALTH_PER_EXTRA_PLAYER
 * percent to the health of a boss: 240 points for one player, 300, 360
 * and 420 for two, three and four, as co-op shooters raise the health of
 * their bosses. */
constexpr int BOSS_HEALTH_PER_EXTRA_PLAYER = 25;

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
