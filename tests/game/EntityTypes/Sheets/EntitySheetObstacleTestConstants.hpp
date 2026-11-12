#pragma once

#include <string_view>

namespace rtype::game::testing {

/** @brief The values of `[gameplay]` of a wall that stays where it appears,
 * and the tables that say what it does to a ship. */
constexpr std::string_view OBSTACLE_GAMEPLAY =
    "[gameplay]\nside = \"obstacle\"\nlayer = \"wall\"\npilot = "
    "\"straight\"\n";
constexpr std::string_view FATAL_OBSTACLE_TABLE =
    "[gameplay.obstacle]\ncontact = \"fatal\"\n";
constexpr std::string_view BLOCKING_OBSTACLE_TABLE =
    "[gameplay.obstacle]\ncontact = \"blocking\"\n";
constexpr std::string_view OBSTACLE_TABLE_OF_AN_UNKNOWN_CONTACT =
    "[gameplay.obstacle]\ncontact = \"bouncy\"\n";
constexpr std::string_view OBSTACLE_TABLE_WITH_AN_UNKNOWN_KEY =
    "[gameplay.obstacle]\ncontact = \"fatal\"\nbounce = 1\n";

/** @brief The rule a sheet breaks when its side and its obstacle table do not
 * go together. */
constexpr std::string_view OBSTACLE_SIDE_RULE_TEXT =
    "a sheet of side obstacle says what it does to a ship in "
    "[gameplay.obstacle], and only such a sheet";

}  // namespace rtype::game::testing
