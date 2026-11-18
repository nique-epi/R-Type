#pragma once

#include <cstddef>
#include <string_view>
#include "Position.hpp"
#include "SpawnOptions.hpp"

namespace rtype::server::testing {

/** @brief Shipped sheets a script names: an enemy it may create, and the ship
 * of a player, which it may neither create nor remove. */
constexpr std::string_view DRIFTER_SHEET = "drifter";
constexpr std::string_view SHIP_SHEET = "ship";
constexpr std::string_view UNKNOWN_SHEET = "no_such_sheet";

/** @brief The Bydo Tyrant, whose sheet gives 240 health points, asked for at
 * 40 percent of them: 96. */
constexpr std::string_view TYRANT_SHEET = "bydo_tyrant";
constexpr game::SpawnOptions AT_FORTY_PERCENT{.healthPercent = 40};
constexpr int FORTY_PERCENT_OF_THE_TYRANT = 96;

/** @brief Where the bodies of the tests appear. */
constexpr game::Position WHERE_A_SCRIPT_PUTS_IT{.x = 900.0F, .y = 300.0F};
constexpr game::Position WHERE_THE_SHIP_FLIES{.x = 200.0F, .y = 500.0F};

/** @brief The bodies, and the removals, a script may ask for in one tick, as
 * the scripting page states. */
constexpr std::size_t SCRIPTED_SPAWNS_IN_A_TICK = 256;
constexpr std::size_t SCRIPTED_REMOVALS_IN_A_TICK = 256;

constexpr std::size_t PLAYERS_TOLD = 2;

/** @brief The name of an event a script publishes. */
constexpr std::string_view SCRIPT_EVENT_NAME = "alarm";

}  // namespace rtype::server::testing
