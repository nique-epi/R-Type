#pragma once

#include <cstddef>
#include <string_view>
#include "ShotPattern.hpp"

namespace rtype::game::testing {

/** @brief A ship armed with player_missile and two weapons: red, a fan of the
 * missile `fan_shot` in two levels, then blue, two rows of player_missile in
 * one level. */
constexpr std::string_view ARMED_SHIP_NAME = "armed_ship";
constexpr std::string_view ARMED_SHIP_TEXT = R"([gameplay]
side = "player"
layer = "player"
pilot = "player"
[gameplay.hitbox]
width = 128
height = 64
[gameplay.motion]
kind = "ship"
[gameplay.gun]
interval_ms = 333
first_shot_ms = 0
projectile = "player_missile"
[[gameplay.weapon]]
name = "red"
[[gameplay.weapon.level]]
interval_ms = 333
projectile = "fan_shot"
count = 3
spread_deg = 24
distance = 16
[[gameplay.weapon.level]]
interval_ms = 250
projectile = "fan_shot"
count = 5
spread_deg = 48
distance = 16
[[gameplay.weapon]]
name = "blue"
[[gameplay.weapon.level]]
interval_ms = 250
projectile = "player_missile"
count = 2
step_y = 24
[appearance]
)";

/** @brief The names and the levels of the weapons of the armed ship, in the
 * order of its sheet, and the shape of the first level of red: 333 ms is 20
 * ticks. */
constexpr std::string_view RED_WEAPON = "red";
constexpr std::string_view BLUE_WEAPON = "blue";
constexpr std::size_t RED_LEVELS = 2;
constexpr std::size_t BLUE_LEVELS = 1;
constexpr std::size_t FIRST_RED_INTERVAL_TICKS = 20;
constexpr ShotPattern FIRST_RED_PATTERN{
    .count = 3, .spreadDegrees = 24.0F, .stepY = 0.0F, .distance = 16.0F};

/** @brief A missile of a player that flies away from where its gun laid it. */
constexpr std::string_view FAN_SHOT_NAME = "fan_shot";
constexpr std::string_view FAN_SHOT_TEXT = R"([gameplay]
side = "player_missile"
layer = "player_missile"
pilot = "scatter"
[gameplay.hitbox]
width = 16
height = 8
[gameplay.motion]
kind = "steady"
top_speed = 960
[appearance]
)";

/** @brief A Bydo that names a weapon, which only a ship may hold. */
constexpr std::string_view ARMED_BYDO_NAME = "armed_bydo";
constexpr std::string_view ARMED_BYDO_TEXT = R"([gameplay]
side = "enemy"
layer = "enemy"
pilot = "straight"
[gameplay.hitbox]
width = 64
height = 64
[gameplay.motion]
kind = "steady"
top_speed = 120
[gameplay.behaviour]
heading = -1
[[gameplay.weapon]]
name = "red"
[[gameplay.weapon.level]]
interval_ms = 333
projectile = "fan_shot"
[appearance]
)";

/** @brief Ships whose weapons break a rule: two weapons named red, a weapon
 * of three levels, a fan of straight missiles, and missiles that scatter laid
 * on the muzzle. */
constexpr std::string_view TWINNED_NAME_SHIP_TEXT = R"([gameplay]
side = "player"
layer = "player"
pilot = "player"
[gameplay.hitbox]
width = 128
height = 64
[gameplay.motion]
kind = "ship"
[[gameplay.weapon]]
name = "red"
[[gameplay.weapon.level]]
interval_ms = 333
projectile = "player_missile"
[[gameplay.weapon]]
name = "red"
[[gameplay.weapon.level]]
interval_ms = 333
projectile = "player_missile"
[appearance]
)";
constexpr std::string_view THREE_LEVEL_SHIP_TEXT = R"([gameplay]
side = "player"
layer = "player"
pilot = "player"
[gameplay.hitbox]
width = 128
height = 64
[gameplay.motion]
kind = "ship"
[[gameplay.weapon]]
name = "red"
[[gameplay.weapon.level]]
interval_ms = 333
projectile = "player_missile"
[[gameplay.weapon.level]]
interval_ms = 333
projectile = "player_missile"
[[gameplay.weapon.level]]
interval_ms = 333
projectile = "player_missile"
[appearance]
)";
constexpr std::string_view STRAIGHT_FAN_SHIP_TEXT = R"([gameplay]
side = "player"
layer = "player"
pilot = "player"
[gameplay.hitbox]
width = 128
height = 64
[gameplay.motion]
kind = "ship"
[[gameplay.weapon]]
name = "red"
[[gameplay.weapon.level]]
interval_ms = 333
projectile = "player_missile"
count = 3
spread_deg = 24
distance = 16
[appearance]
)";
constexpr std::string_view SCATTER_ON_THE_MUZZLE_SHIP_TEXT = R"([gameplay]
side = "player"
layer = "player"
pilot = "player"
[gameplay.hitbox]
width = 128
height = 64
[gameplay.motion]
kind = "ship"
[[gameplay.weapon]]
name = "red"
[[gameplay.weapon.level]]
interval_ms = 333
projectile = "fan_shot"
count = 3
spread_deg = 24
[appearance]
)";

}  // namespace rtype::game::testing
