#pragma once

#include <cstdint>
#include "Position.hpp"

namespace rtype::client::testing {

constexpr std::uint32_t REVEALED_MISSILE_ID = 21;

/** @brief The first state that lists the missile, and the next one, 3 ticks
 * later as the server sends 20 states per second. Once both arrived, the
 * clock shows two intervals behind the newest: tick 27. */
constexpr std::uint32_t FIRST_MISSILE_STATE_TICK = 30;
constexpr std::uint32_t SECOND_MISSILE_STATE_TICK = 33;

/** @brief The missile leaves the muzzle at x 400 and flies 16 units a tick,
 * the 960 units per second of the shipped player missile. */
constexpr game::Position MISSILE_AT_THE_MUZZLE{.x = 400.0F, .y = 300.0F};
constexpr game::Position MISSILE_THREE_TICKS_LATER{.x = 448.0F, .y = 300.0F};

/** @brief The frames a test shows after both states arrived, one tick each:
 * from tick 28 to tick 36, past the first state and past the second. */
constexpr int FRAMES_SHOWN = 9;

/** @brief What a test reads for a missile the registry does not hold: a
 * position no test expects. */
constexpr game::Position NO_MISSILE_POSITION{.x = -1.0e9F, .y = -1.0e9F};

}  // namespace rtype::client::testing
