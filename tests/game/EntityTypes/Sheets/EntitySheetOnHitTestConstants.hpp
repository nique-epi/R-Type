#pragma once

#include <string_view>

namespace rtype::game::testing {

/** @brief The hit handler the registry of the tests holds, and the values of
 * `[gameplay]` of a wall and of a ship that name it, or name another. */
constexpr std::string_view ON_HIT_HANDLER_NAME = "crack";
constexpr std::string_view WALL_GAMEPLAY_NAMING_THE_HANDLER =
    "[gameplay]\nside = \"obstacle\"\nlayer = \"wall\"\npilot = "
    "\"straight\"\non_hit = \"crack\"\n";
constexpr std::string_view WALL_GAMEPLAY_NAMING_AN_UNKNOWN_HANDLER =
    "[gameplay]\nside = \"obstacle\"\nlayer = \"wall\"\npilot = "
    "\"straight\"\non_hit = \"shatter\"\n";
constexpr std::string_view SHIP_GAMEPLAY_NAMING_THE_HANDLER =
    "[gameplay]\nside = \"player\"\nlayer = \"player\"\npilot = "
    "\"player\"\non_hit = \"crack\"\n";

/** @brief The rule a ship that names an on_hit breaks. */
constexpr std::string_view ON_HIT_SIDE_RULE_TEXT =
    "a ship answers a hit with its lives, never an on_hit";

}  // namespace rtype::game::testing
