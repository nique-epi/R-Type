#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>
#include "Position.hpp"

namespace rtype::server::testing {

/** @brief The sheets of the boss stage tests: a boss that stays where it
 * appears, with 20 points, and a minion. */
constexpr std::string_view WARLORD_NAME_IN_TESTS = "warlord";
constexpr std::string_view STAGE_MINION_NAME_IN_TESTS = "minion";
constexpr std::string_view WARLORD_SHEET_TEXT = R"([gameplay]
side = "enemy"
layer = "enemy"
pilot = "straight"
[gameplay.hitbox]
width = 96
height = 96
[gameplay.motion]
kind = "steady"
top_speed = 240
[gameplay.behaviour]
heading = 0
[gameplay.health]
points = 20
[gameplay.boss]
rage_below_percent = 50
rage_attack = 9
arrival_attack = 0
[appearance]
)";
constexpr std::string_view STAGE_MINION_SHEET_TEXT = R"([gameplay]
side = "enemy"
layer = "enemy"
pilot = "straight"
[gameplay.hitbox]
width = 64
height = 64
[gameplay.motion]
kind = "steady"
top_speed = 240
[gameplay.behaviour]
heading = 0
[appearance]
)";

/** @brief The network id the recorded orders give every body. */
constexpr std::uint32_t ORDERED_BOSS_ID = 77;

/** @brief Where the boss of the waves is ordered: half its 512-unit frame
 * past the right edge of the 1920-unit playfield, on the middle row of the
 * 984 units of the area of play. */
constexpr game::Position WHERE_THE_BOSS_IS_ORDERED{.x = 2176.0F, .y = 492.0F};

/** @brief The tick the last wave is found over orders the boss, and the game
 * makes it appear at the end of the next one. */
constexpr std::size_t TICKS_TO_ORDER_THE_BOSS = 1;

/** @brief A few ticks: as many while the waves go on, or the boss lives. */
constexpr std::size_t A_FEW_STAGE_TICKS = 10;

}  // namespace rtype::server::testing
