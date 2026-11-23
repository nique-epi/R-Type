#pragma once

#include <string_view>

namespace rtype::game::testing {

/** @brief A crystal of the weapon red, which the armed ship of the weapon
 * tests holds, drifting left. */
constexpr std::string_view RED_CRYSTAL_NAME = "red_crystal";
constexpr std::string_view RED_CRYSTAL_TEXT = R"([gameplay]
side = "crystal"
layer = "crystal"
hits = ["player"]
crystal = "red"
pilot = "straight"
[gameplay.hitbox]
width = 48
height = 32
[gameplay.motion]
kind = "steady"
top_speed = 120
[gameplay.behaviour]
heading = -1
[appearance]
)";

/** @brief A crystal of a weapon no ship holds. */
constexpr std::string_view GREEN_CRYSTAL_TEXT = R"([gameplay]
side = "crystal"
layer = "crystal"
crystal = "green"
pilot = "straight"
[gameplay.hitbox]
width = 48
height = 32
[gameplay.motion]
kind = "steady"
top_speed = 120
[gameplay.behaviour]
heading = -1
[appearance]
)";

/** @brief A crystal that drifts right, away from the edge that removes it. */
constexpr std::string_view RIGHTWARD_CRYSTAL_TEXT = R"([gameplay]
side = "crystal"
layer = "crystal"
crystal = "red"
pilot = "straight"
[gameplay.hitbox]
width = 48
height = 32
[gameplay.motion]
kind = "steady"
top_speed = 120
[gameplay.behaviour]
heading = 1
[appearance]
)";

/** @brief A Bydo that drops the red crystal, and one that drops the missile
 * of a player. */
constexpr std::string_view CARRIER_NAME = "carrier";
constexpr std::string_view CARRIER_TEXT = R"([gameplay]
side = "enemy"
layer = "enemy"
pilot = "straight"
drops = ["red_crystal"]
[gameplay.hitbox]
width = 64
height = 64
[gameplay.motion]
kind = "steady"
top_speed = 120
[gameplay.behaviour]
heading = -1
[appearance]
)";
constexpr std::string_view MISSILE_CARRIER_TEXT = R"([gameplay]
side = "enemy"
layer = "enemy"
pilot = "straight"
drops = ["player_missile"]
[gameplay.hitbox]
width = 64
height = 64
[gameplay.motion]
kind = "steady"
top_speed = 120
[gameplay.behaviour]
heading = -1
[appearance]
)";

/** @brief A missile of an enemy that drops the red crystal, which only an
 * enemy may. */
constexpr std::string_view DROPPING_MISSILE_NAME = "dropping_missile";
constexpr std::string_view DROPPING_MISSILE_TEXT = R"([gameplay]
side = "enemy_missile"
layer = "enemy_missile"
pilot = "straight"
drops = ["red_crystal"]
[gameplay.hitbox]
width = 16
height = 16
[gameplay.motion]
kind = "steady"
top_speed = 360
[gameplay.behaviour]
heading = -1
[appearance]
)";

}  // namespace rtype::game::testing
