#pragma once

#include <cstddef>
#include <string_view>
#include "Position.hpp"

namespace rtype::server::testing {

/** @brief Shipped sheets a script names: an enemy it may create, and the ship
 * of a player, which it may neither create nor remove. */
constexpr std::string_view DRIFTER_SHEET = "drifter";
constexpr std::string_view SHIP_SHEET = "ship";
constexpr std::string_view UNKNOWN_SHEET = "no_such_sheet";

/** @brief Where the bodies of the tests appear. */
constexpr game::Position WHERE_A_SCRIPT_PUTS_IT{.x = 900.0F, .y = 300.0F};
constexpr game::Position WHERE_THE_SHIP_FLIES{.x = 200.0F, .y = 500.0F};

/** @brief The bodies, and the removals, a script may ask for in one tick, as
 * the scripting page states. */
constexpr std::size_t SCRIPTED_SPAWNS_IN_A_TICK = 256;
constexpr std::size_t SCRIPTED_REMOVALS_IN_A_TICK = 256;

constexpr std::size_t PLAYERS_TOLD = 2;

}  // namespace rtype::server::testing
