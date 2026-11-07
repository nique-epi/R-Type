#pragma once

#include <chrono>
#include <cstddef>
#include "LagCompensationConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

/** @brief The furthest in the past FavorTheShooter tests a missile: 200 ms,
 * about the 250 ms Overwatch allows, so a shooter with a long ping cannot hit
 * what another player left long ago. */
constexpr engine::Duration SHOOTER_REWIND = std::chrono::milliseconds{200};

/** @brief The whole ticks of SHOOTER_REWIND: 12 at 60 ticks per second. */
constexpr std::size_t SHOOTER_REWIND_TICKS =
    engine::wholeTicksIn(SHOOTER_REWIND);

/** @brief The furthest in the past FavorThePlayers tests a missile or a ship:
 * the longest the game keeps, 400 ms, since a player at a round trip of
 * 100 ms sees the enemies 17 ticks behind what the server tests, and one at
 * 200 ms 23 ticks; no player loses what a Bydo cannot hit. */
constexpr std::size_t PLAYERS_REWIND_TICKS = game::MAXIMUM_REWIND_TICKS;

static_assert(SHOOTER_REWIND_TICKS <= game::MAXIMUM_REWIND_TICKS,
              "the game must keep the enemies a mode looks back to");

}  // namespace rtype::server
