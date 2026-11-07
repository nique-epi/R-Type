#pragma once

#include <string_view>

namespace rtype::game::testing {

/** @brief The sheet of an enemy whose volley, in the strike of its attack 1,
 * is made of three bodies of the sheet `minion`; of the minion, an enemy;
 * and of a brooder whose volley is made of missiles of a player. */
constexpr std::string_view BROODER_NAME_IN_SHEET_TESTS = "brooder";
constexpr std::string_view MINION_NAME_IN_SHEET_TESTS = "minion";
constexpr std::string_view BROODER_OF_MINIONS_TEXT = R"([gameplay]
side = "enemy"
pilot = "straight"
[gameplay.hitbox]
width = 96
height = 96
[gameplay.motion]
kind = "steady"
top_speed = 240
[gameplay.behaviour]
heading = -1
[[gameplay.volley]]
attack = 1
phase = "strike"
body = "minion"
count = 3
offset_x = -100
offset_y = 0
[appearance]
)";
constexpr std::string_view MINION_SHEET_TEXT = R"([gameplay]
side = "enemy"
pilot = "straight"
[gameplay.hitbox]
width = 64
height = 64
[gameplay.motion]
kind = "steady"
top_speed = 240
[gameplay.behaviour]
heading = -1
[appearance]
)";
constexpr std::string_view BROODER_OF_PLAYER_MISSILES_TEXT = R"([gameplay]
side = "enemy"
pilot = "straight"
[gameplay.hitbox]
width = 96
height = 96
[gameplay.motion]
kind = "steady"
top_speed = 240
[gameplay.behaviour]
heading = -1
[[gameplay.volley]]
attack = 1
phase = "strike"
body = "player_missile"
count = 3
offset_x = -100
offset_y = 0
[appearance]
)";

}  // namespace rtype::game::testing
