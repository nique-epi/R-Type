#pragma once

#include <string_view>

namespace rtype::game::testing {

/** @brief The table of a missile that takes three health points, and of one
 * that would take none, which the range of the key refuses. */
constexpr std::string_view MISSILE_DAMAGE_TABLE =
    "[gameplay.damage]\npoints = 3\n";
constexpr std::string_view ZERO_DAMAGE_TABLE =
    "[gameplay.damage]\npoints = 0\n";
constexpr int MISSILE_DAMAGE_POINTS = 3;

/** @brief The rule a sheet that is no missile and names a damage breaks. */
constexpr std::string_view DAMAGE_SIDE_RULE_TEXT =
    "only a missile deals damage";

}  // namespace rtype::game::testing
