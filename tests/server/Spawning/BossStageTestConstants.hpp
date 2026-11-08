#pragma once

#include <cstddef>
#include <string_view>
#include "BossStageConstants.hpp"

namespace rtype::server::testing {

/** @brief The sheets of the boss stage tests: a boss that stays where it
 * appears, with 20 points, and a minion. */
constexpr std::string_view WARLORD_NAME_IN_TESTS = "warlord";
constexpr std::string_view STAGE_MINION_NAME_IN_TESTS = "minion";
constexpr std::string_view WARLORD_SHEET_TEXT = R"([gameplay]
side = "enemy"
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

/** @brief The tick the last wave is found over announces the boss; it
 * appears BOSS_ARRIVAL_WARNING_TICKS ticks later, on the next tick. */
constexpr std::size_t TICKS_TO_ANNOUNCE_THE_BOSS = 1;
constexpr std::size_t TICKS_TO_THE_LAST_TICK_OF_THE_WARNING =
    TICKS_TO_ANNOUNCE_THE_BOSS + BOSS_ARRIVAL_WARNING_TICKS;
constexpr std::size_t TICKS_TO_THE_BOSS =
    TICKS_TO_THE_LAST_TICK_OF_THE_WARNING + 1;

/** @brief A few ticks: as many while the waves go on, or the boss lives. */
constexpr std::size_t A_FEW_STAGE_TICKS = 10;
/** @brief Longer than the warning: a level without boss announces none. */
constexpr std::size_t LONGER_THAN_THE_WARNING = BOSS_ARRIVAL_WARNING_TICKS * 2;

/** @brief The players told about the boss, by the order they connect in. */
constexpr std::size_t FIRST_STAGE_PLAYER = 0;
constexpr std::size_t SECOND_STAGE_PLAYER = 1;
constexpr std::size_t STAGE_PLAYER_COUNT = 2;

}  // namespace rtype::server::testing
