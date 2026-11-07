#pragma once

#include <cstddef>
#include <cstdint>
#include "Position.hpp"

namespace rtype::server::testing {

/** @brief The tick of the tests, a tick five before it that a client showed,
 * and how far behind that is. */
constexpr std::uint32_t PRESENT_TICK = 100;
constexpr std::uint32_t TICK_FIVE_BEFORE = 95;
constexpr std::size_t FIVE_TICKS_BEHIND = 5;

/** @brief The inputNumber of the input applied last to the ship of the
 * player who fires. */
constexpr std::uint32_t SHOOTER_INPUT_NUMBER = 41;

/** @brief A tick thirty before the present that a client showed, past what
 * either mode looks back: 200 ms (12 ticks) between players, 400 ms (24 ticks)
 * in co-op. */
constexpr std::uint32_t TICK_THIRTY_BEFORE = 70;
constexpr std::size_t SHOOTER_REWIND_IN_TICKS = 12;
constexpr std::size_t PLAYERS_REWIND_IN_TICKS = 24;

/** @brief The session token of the player who fires. */
constexpr std::uint64_t SHOOTER_SESSION_TOKEN = 0x5115'7E25;

/** @brief Where the ship of the tests stands. */
constexpr game::Position SOME_SHIP_PLACE{.x = 300.0F, .y = 300.0F};

/** @brief Where the missile of the tests flies. */
constexpr game::Position SOME_MISSILE_PLACE{.x = 600.0F, .y = 300.0F};

}  // namespace rtype::server::testing
